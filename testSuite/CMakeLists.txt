# Define Macro for adding tests

IF (NOT ${PROJECT_NAME} STREQUAL "Mesquite")
ENDIF()


IF(${PACKAGE_NAME}_ENABLE_SHARED)
  SET( TEST_DEFINES "${MESQUITE_DEFINES}" )
ELSE()
  SET( TEST_DEFINES "${MESQUITE_DEFINES} -DMESQUITE_STATIC_LIB" )
ENDIF()

MACRO(MSQ_ADD_TEST_PARALLEL name exe nproc)
  IF (NOT ${PROJECT_NAME} STREQUAL "Mesquite")
    TRIBITS_ADD_EXECUTABLE( ${exe} SOURCES ${ARGN} )
    TRIBITS_ADD_TEST( ${exe} NAME ${name} NUM_MPI_PROCS ${nproc} )
    #TARGET_LINK_LIBRARIES( ${exe} mesquite )
  ELSE(NOT ${PROJECT_NAME} STREQUAL "Mesquite")
    IF (${PACKAGE_NAME}_ENABLE_TESTS)
      ADD_EXECUTABLE( ${exe} ${ARGN} )
      ADD_TEST( ${name} ${EXECUTABLE_OUTPUT_PATH}/${exe} )
      TARGET_LINK_LIBRARIES( ${exe} mesquite )
    ENDIF(${PACKAGE_NAME}_ENABLE_TESTS)
  ENDIF(NOT ${PROJECT_NAME} STREQUAL "Mesquite")
  SET_SOURCE_FILES_PROPERTIES(${ARGN} COMPILE_FLAGS "-DSRCDIR=\"\\\"${CMAKE_CURRENT_SOURCE_DIR}/\\\"\" ${TEST_DEFINES}")
ENDMACRO(MSQ_ADD_TEST_PARALLEL)

MACRO(MSQ_ADD_TEST name exe)
  MSQ_ADD_TEST_PARALLEL( ${name} ${exe} 1 ${ARGN} )
ENDMACRO(MSQ_ADD_TEST)


# Create meshfiles.h

IF(NOT DEFINED ${PACKAGE_SOURCE_DIR})
  SET(PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
ENDIF()
SET(meshfiles_h_contents "#ifndef MESH_FILES_DIR\n# define MESH_FILES_DIR \"${PACKAGE_SOURCE_DIR}/meshFiles/\"\n#endif\n" )
IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/meshfiles.h)
  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/meshfiles.h ${meshfiles_h_contents} )
ENDIF()
TRIBITS_INCLUDE_DIRECTORIES(REQUIRED_DURING_INSTALLATION_TESTING ${CMAKE_CURRENT_BINARY_DIR})



# All tests are in subdirs

ADD_SUBDIRECTORY(2d_formulation)
ADD_SUBDIRECTORY(2d_metrics)
ADD_SUBDIRECTORY(2d_target)
ADD_SUBDIRECTORY(ActiveSetTest)
ADD_SUBDIRECTORY(algorithm_test)
ADD_SUBDIRECTORY(analytical_grad_3D)
ADD_SUBDIRECTORY(benchmark_tests)
ADD_SUBDIRECTORY(headers)
ADD_SUBDIRECTORY(high_aspect_ratio)
ADD_SUBDIRECTORY(higher_order)
ADD_SUBDIRECTORY(idft_time)
ADD_SUBDIRECTORY(laplacian_test)
ADD_SUBDIRECTORY(laplacian_polygon_test)
ADD_SUBDIRECTORY(nongradient_test)
ADD_SUBDIRECTORY(paraboloid_domain_test)
ADD_SUBDIRECTORY(pyramid)
ADD_SUBDIRECTORY(simple_hybrid_test)
ADD_SUBDIRECTORY(slaved)
ADD_SUBDIRECTORY(synchronous)
ADD_SUBDIRECTORY(test_1)
ADD_SUBDIRECTORY(transform)
ADD_SUBDIRECTORY(tutorial)
ADD_SUBDIRECTORY(untangle_test)
ADD_SUBDIRECTORY(wedge)
ADD_SUBDIRECTORY(wrapper_tests)

IF (TPL_ENABLE_MPI)
  ADD_SUBDIRECTORY(parallel_untangle_shape)
  ADD_SUBDIRECTORY(parallel_smooth_laplace)
ENDIF()

IF(${PACKAGE_NAME}_ENABLE_IMESH)
  ADD_SUBDIRECTORY(imesh)
ENDIF()

IF(${PACKAGE_NAME}_ENABLE_IGEOM)
  ADD_SUBDIRECTORY(igeom)
ENDIF()

IF (NOT ${PROJECT_NAME} STREQUAL "Mesquite")
  IF(${PACKAGE_NAME}_ENABLE_CppUnit)
    SET(CPPUNIT_LIBRARY ${TPL_CppUnit_LIBRARIES})
    SET(CPPUNIT_INCLUDES ${TPL_CppUnit_INCLUDE_DIRS})
  ELSE()
    SET(CPPUNIT_LIBRARY "CPPUNIT_LIBRARY-NOTFOUND")
    SET(CPPUNIT_INCLUDES "CPPUNIT_INCLUDES-NOTFOUND")
  ENDIF()
ELSE()
  # Check for libraries
  FIND_LIBRARY( CPPUNIT_LIBRARY NAMES cppunit )
  FIND_PATH( CPPUNIT_INCLUDES cppunit/extensions/HelperMacros.h )
ENDIF()

# Cannot build unit tests with Windows DLL because
# internal symbols are not exposed
IF (EXISTS ${CPPUNIT_LIBRARY} AND IS_DIRECTORY ${CPPUNIT_INCLUDES})
  IF(WIN32 OR WIN64)
    IF(NOT ${PACKAGE_NAME}_ENABLE_SHARED)
      ADD_SUBDIRECTORY(unit)
    ENDIF()
  ELSE()
    ADD_SUBDIRECTORY(unit)
  ENDIF()
ENDIF()
