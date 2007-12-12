/* ***************************************************************** 
    MESQUITE -- The Mesh Quality Improvement Toolkit

    Copyright 2007 Sandia National Laboratories.  Developed at the
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

    (2007) kraftche@cae.wisc.edu    

  ***************************************************************** */


/** \file XYRectangle.cpp
 *  \brief 
 *  \author Jason Kraftcheck 
 */

#include "Mesquite.hpp"
#include "XYRectangle.hpp"
#include "MsqError.hpp"
#include "MsqVertex.hpp"

namespace Mesquite {


XYRectangle::XYRectangle( double w, double h, double x, double y )
{
  minCoords[0] = x;
  minCoords[1] = y;
  maxCoords[0] = x+w;
  maxCoords[1] = y+w;
}

void XYRectangle::setup( Mesh* mesh, MsqError& err )
{
  const double epsilon = 1e-4;
  if (maxCoords[0] - minCoords[0] <= epsilon || 
      maxCoords[1] - minCoords[1] <= epsilon) {
    MSQ_SETERR(err)("Invalid rectangle dimensions", MsqError::INVALID_STATE);
    return;
  }
  
  msq_std::vector<Mesh::EntityHandle> vertices;
  mesh->get_all_vertices( vertices, err ); MSQ_ERRRTN(err);
  
  msq_std::vector<MsqVertex> coords(vertices.size());
  mesh->vertices_get_coordinates( &vertices[0], &coords[0], coords.size(), err ); MSQ_ERRRTN(err);
  
  for (size_t i = 0; i < vertices.size(); ++i) {
    for (int d = 0; d < 2; ++d) {
      if (minCoords[d] - coords[i][d] > epsilon || 
          coords[i][d] - maxCoords[d] > epsilon) {
        MSQ_SETERR(err)(MsqError::INVALID_MESH,
                        "Invalid vertex coordinate: (%f,%f,%f)\n",
                        coords[i][0],coords[i][1],coords[i][2]);
        return;
      }
      else if (coords[i][d] - minCoords[d] < epsilon) {
        VertexConstraint c( d, minCoords[d] );
        mConstraints.insert( constraint_t::value_type( vertices[i], c ) );
      }
      else if (maxCoords[d] - coords[i][d] < epsilon) {
        VertexConstraint c( d, maxCoords[d] );
        mConstraints.insert( constraint_t::value_type( vertices[i], c ) );
      }
    }
  }
}

void XYRectangle::snap_to( Mesh::EntityHandle entity_handle,
                           Vector3D &coordinate ) const
{
    // everything gets moved into the plane
  coordinate[2] = 0;
    // apply other constraints
  constraint_t::const_iterator i = mConstraints.lower_bound( entity_handle );
  for (; i != mConstraints.end() && i->first == entity_handle; ++i)
    coordinate[i->second.axis] = i->second.coord;
}
  
void XYRectangle::normal_at( Mesh::EntityHandle handle, Vector3D &norm ) const
{
  norm.set(0,0,1);
}

void XYRectangle::normal_at( const Mesh::EntityHandle* handles,
                             Vector3D normals[],
                             unsigned count,
                             MsqError&  ) const
{
  for (unsigned i = 0; i < count; ++i)
    normal_at( handles[i], normals[i] );
}
    
void XYRectangle::closest_point( Mesh::EntityHandle handle,
                                 const Vector3D& position,
                                 Vector3D& closest,
                                 Vector3D& normal,
                                 MsqError&  ) const
{
  normal = position;
  normal_at( handle, normal );
  closest = position;
  snap_to( handle, closest );
}
    
void XYRectangle::domain_DoF( const Mesh::EntityHandle* handle_array,
                              unsigned short* dof_array,
                              size_t num_handles,
                              MsqError&  ) const
{
  for (unsigned i = 0; i < num_handles; ++i) {
      // everything is at least constrained to XY-plane
    dof_array[i] = 2;
      // each additional constraint reduces degrees of freedom
    constraint_t::const_iterator j = mConstraints.lower_bound( handle_array[i] );
    for (; j != mConstraints.end() && j->first == handle_array[i]; ++j)
      --dof_array[i];
  }
}

} // namespace Mesquite