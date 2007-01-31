/* ***************************************************************** 
    MESQUITE -- The Mesh Quality Improvement Toolkit

    Copyright 2006 Sandia National Laboratories.  Developed at the
    University of Wisconsin--Madison under SNL contract number
    624796.  The U.S. Government and the University of Wisconsin
    retain certain rights to this software.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License 
    (lgpl.txt) along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    (2006) kraftche@cae.wisc.edu
   
  ***************************************************************** */


/** \file JacobianMetric.cpp
 *  \brief 
 *  \author Jason Kraftcheck 
 */

#include "Mesquite.hpp"
#include "JacobianMetric.hpp"
#include "SamplePoints.hpp"
#include "MsqMatrix.hpp"
#include "ElementQM.hpp"
#include "MsqError.hpp"
#include "Vector3D.hpp"
#include "PatchData.hpp"
#include "MappingFunction.hpp"
#include "WeightCalculator.hpp"
#include "TargetCalculator.hpp"
#include "TargetMetric2D.hpp"
#include "TargetMetric3D.hpp"
#include "TargetMetricUtil.hpp"

#ifdef MSQ_USE_OLD_STD_HEADERS
# include <functional.h>
# include <algorithm.h>
#else
# include <functional>
# include <algorithm>
#endif

namespace Mesquite {

int JacobianMetric::get_negate_flag( ) const { return 1; }

msq_std::string JacobianMetric::get_name() const
  { return msq_std::string("JacobianMetric"); }

void JacobianMetric::get_evaluations( PatchData& pd,
                                      msq_std::vector<size_t>& handles,
                                      bool free,
                                      MsqError& err )
{
  get_sample_pt_evaluations( pd, samplePts, handles, free, err );
}

void JacobianMetric::get_element_evaluations( PatchData& pd,
                                              size_t elem,
                                              msq_std::vector<size_t>& handles,
                                              MsqError& err )
{
  get_elem_sample_points( pd, samplePts, elem, handles, err );
}

bool JacobianMetric::evaluate( PatchData& pd, size_t handle, double& value, MsqError& err )
{
  mIndices.clear();
  return evaluate_with_indices( pd, handle, value, mIndices, err );
}

bool JacobianMetric::evaluate_with_indices( PatchData& pd,
                                            size_t handle,
                                            double& value,
                                            msq_std::vector<size_t>& indices,
                                            MsqError& err )
{
  unsigned s = ElemSampleQM::sample( handle );
  size_t   e = ElemSampleQM::  elem( handle );
  MsqMeshEntity& elem = pd.element_by_index( e );
  EntityTopology type = elem.get_element_type();
  unsigned dim, num;
  samplePts->location_from_sample_number( type, s, dim, num );
  unsigned edim = TopologyInfo::dimension( type );
  const size_t* conn = elem.get_vertex_index_array();
  
  unsigned bits = pd.higher_order_node_bits( e );
  
  const MappingFunction* func = pd.get_mapping_function( type );
  if (!func) {
    MSQ_SETERR(err)( "No mapping function for element type", MsqError::UNSUPPORTED_ELEMENT );
    return false;
  }
  
  indices.clear();
  mDerivs.clear();
  switch (dim) {
    case 0:
      func->derivatives_at_corner( num, bits, indices, mDerivs, err );
      break;
    case 1:
      func->derivatives_at_mid_edge( num, bits, indices, mDerivs, err );
      break;
    case 2:
      if (edim != 2) {
        func->derivatives_at_mid_face( num, bits, indices, mDerivs, err );
        break;
      }
    case 3:
      func->derivatives_at_mid_elem( bits, indices, mDerivs, err );
      break;
  }
  MSQ_ERRZERO( err );
  std::vector<double>::const_iterator d = mDerivs.begin();
  
    // Convert from indices into element connectivity list to
    // indices into vertex array in patch data.
  for (msq_std::vector<size_t>::iterator i = indices.begin(); i != indices.end(); ++i)
    *i = conn[*i];
  
  bool rval;
  if (edim == 3) { // 3x3 or 3x2 targets ?
    if (!metric3D) {
      MSQ_SETERR(err)("No 3D metric for Jacobian-based metric.\n", MsqError::UNSUPPORTED_ELEMENT );
      return false;
    }
  
    Vector3D c[3] = { Vector3D(0,0,0), Vector3D(0,0,0), Vector3D(0,0,0) };
    for (size_t i = 0; i < indices.size(); ++i) {
      Vector3D coords = pd.vertex_by_index( indices[i] );
      c[0] += *d * coords; ++d;
      c[1] += *d * coords; ++d;
      c[2] += *d * coords; ++d;
    }
    MsqMatrix<3,3> A( (MsqMatrix<3,1>*)c );

    MsqMatrix<3,3> W;
    targetCalc->get_3D_target( pd, e, samplePts, s, W, err ); MSQ_ERRZERO(err);
    rval = metric3D->evaluate( A, W, value, err ); MSQ_ERRZERO(err);
  }
  else {
    if (!metric2D) {
      MSQ_SETERR(err)("No 2D metric for Jacobian-based metric.\n", MsqError::UNSUPPORTED_ELEMENT );
      return false;
    }
  
    Vector3D c[2] = { Vector3D(0,0,0), Vector3D(0,0,0) };
    for (size_t i = 0; i < indices.size(); ++i) {
      Vector3D coords = pd.vertex_by_index( indices[i] );
      c[0] += *d * coords; ++d;
      c[1] += *d * coords; ++d;
    }
    MsqMatrix<3,2> App( (MsqMatrix<3,1>*)c );
    
    
    MsqMatrix<3,2> Wp;
    targetCalc->get_2D_target( pd, e, samplePts, s, Wp, err ); MSQ_ERRZERO(err);
    
    MsqMatrix<2,2> A, W;
    surface_to_2d( App, Wp, A, W );
    rval = metric2D->evaluate( A, W, value, err ); MSQ_ERRZERO(err);
  }
  
    // apply target weight to value
  double ck = weightCalc->get_weight( pd, e, samplePts, s, err ); MSQ_ERRZERO(err);
  value *= ck;
  
    // remove indices for non-free vertices
  indices.erase( msq_std::remove_if( indices.begin(), indices.end(), 
    msq_std::bind2nd(msq_std::greater_equal<size_t>(),pd.num_free_vertices())),
    indices.end() );
  
  return rval;
}




} // namespace Mesquite
