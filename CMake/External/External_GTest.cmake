#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
include(imstkAddExternalProject)
imstk_add_external_project( GTest
  URL https://github.com/google/googletest/archive/release-1.10.0.zip
  URL_MD5 82358affdd7ab94854c8ee73a180fc53
  CMAKE_CACHE_ARGS
    -DBUILD_GMOCK:BOOL=ON
    -DBUILD_GTEST:BOOL=ON
    -DBUILD_SHARED_LIBS:BOOL=ON
  DEPENDENCIES ""
  RELATIVE_INCLUDE_PATH ""
  #VERBOSE
  )
if(NOT USE_SYSTEM_GTest)
  set(GTest_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/GTest)
  #message(STATUS "GTest_DIR : ${GTest_DIR}")
endif()