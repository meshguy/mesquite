// -*- Mode : c++; tab-width: 3; c-tab-always-indent: t; indent-tabs-mode: nil; c-basic-offset: 3 -*-

/*! \file UntangleBetaQualityMetric.hpp

Header file for the Mesquite::UntangleBetaQualityMetric class

  \author Michael Brewer
  \date   2002-09-10
 */

#ifndef UNTANGLE_BETA_QUALITY_METRIC_HPP
#define UNTANGLE_BETA_QUALITY_METRIC_HPP

#include "Mesquite.hpp"
#include "UntangleQualityMetric.hpp"

namespace Mesquite
{
     /*! \class UntangleBetaQualityMetric
       Given a scalar value beta and local signed element volume alpha_i,
       define delta_i to be alpha_i minus beta.  The Untangle beta value
       is then defined as square root of the sum over sample points
       of the absolute value of delta_i minus delta_i, difference squared.
       That is, the root mean square of the difference, abs(delta_i) minus
       delta_i.
     */
   
   class UntangleBetaQualityMetric : public UntangleQualityMetric
   {
   public:
     
       /*! \fn UntangleQualityMetric* UntangleBetaQualityMetric::create_new()
         \brief The function create_new is used to create a untangle quality
         metric.  The constructor defaults to RMS AveragingMethod and
         ELEMENT_VERTICES evaluationMode.  The default beta value is
         .05.
       */
     static UntangleQualityMetric* create_new()
        {
          UntangleQualityMetric* m = new UntangleBetaQualityMetric(.05);
          return m;
        }
       /*! \fn UntangleQualityMetric* UntangleBetaQualityMetric::create_new(double bet)
         \brief The function create_new is used to create a untangle quality
         metric.  The constructor defaults to RMS AveragingMethod and
         ELEMENT_VERTICES evaluationMode.  mBeta, the scalar value used
         to compute the metric value, is set to 'bet'.
       */
     static UntangleQualityMetric* create_new(double bet)
        {
          UntangleQualityMetric* m = new UntangleBetaQualityMetric(bet);
          return m;
        }
     
       // virtual destructor ensures use of polymorphism during destruction
     virtual ~UntangleBetaQualityMetric()
        {}
       /*!Evaluate the Untangle Beta metric value for an element.
         \todo This function needs to be modifies so that it no longer
         uses compute_weighted_jacobian.  It also needs to set an error
         whenever sent a 2D element and the surface normal information
         is not available.*/
     bool evaluate_element(PatchData &pd,MsqMeshEntity* element,
                           double &fval,MsqError &err);
     
   protected:
     
     
   private:
     double mBeta;
     UntangleBetaQualityMetric(double bet);
   };


} //namespace


#endif // UntangleBetaQualityMetric_hpp


