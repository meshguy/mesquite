// -*- Mode : c++; tab-width: 3; c-tab-always-indent: t; indent-tabs-mode: nil; c-basic-offset: 3 -*-
//
//   SUMMARY: 
//     USAGE:
//
//    AUTHOR: Thomas Leurent <tleurent@mcs.anl.gov>
//       ORG: Argonne National Laboratory
//    E-MAIL: tleurent@mcs.anl.gov
//
// ORIG-DATE: 13-Jan-03 at 09:05:56
//  LAST-MOD: 17-Jan-03 at 18:08:40 by Thomas Leurent
//
// DESCRIPTION:
// ============
/*! \file MsqHessianTest.cpp

Unit testing of the MsqHessian class. 

*/
// DESCRIP-END.
//


#include "PatchDataInstances.hpp"
#include "MsqHessian.hpp"

#include <math.h>

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/SignalException.h"

using namespace Mesquite;
using std::cout;
using std::cerr;
using std::endl;

class MsqHessianTest
  : public CppUnit::TestFixture,
    public Mesquite::MsqHessian
{

private:
  CPPUNIT_TEST_SUITE(MsqHessianTest);
  CPPUNIT_TEST (test_initialize);
  CPPUNIT_TEST_SUITE_END();

private:
   
  PatchData twoTriangles; 

public:
  void setUp()
  {
    MsqError err;
    create_two_tri_patch(twoTriangles, err); MSQ_CHKERR(err);
  }

  void tearDown()
  {
  }
  
public:
  MsqHessianTest()
  {}

  void test_initialize()
  {
    MsqError err;
    
    MsqHessian::initialize(twoTriangles, err); MSQ_CHKERR(err);

    // Checks values of mRowStart are correct.
    int row_start[] = {0, 4, 7, 8, 9};
    for (int i=0; i<5; ++i) 
      CPPUNIT_ASSERT( mRowStart[i] == row_start[i] );

    // Checks values of mColIndex are correct.
    int col_index[] = {0,1,2,3,1,2,3,2,3};
    for (int i=0; i<9; ++i) 
      CPPUNIT_ASSERT( mColIndex[i] == col_index[i] );

    // Checks values of mAccumulation are correct. 
    int col_instr[] = {0,1,2,4,5,7,0,3,1,8,-6,4};
    for (int i=0; i<12; ++i)
      CPPUNIT_ASSERT( mAccumulation[i] == col_instr[i] );

  }

};


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(MsqHessianTest, "MsqHessianTest");
 
