#-----------------------------------------------------------------------------
# Add External Project
#-----------------------------------------------------------------------------
include(imstkAddExternalProject)
imstk_add_external_project( g3log
  GIT_REPOSITORY https://gitlab.kitware.com/iMSTK/g3log.git
  GIT_TAG 6c1698c4f7db6b9e4246ead38051f9866ea3ac06
  CMAKE_CACHE_ARGS
    -DADD_FATAL_EXAMPLE:BOOL=OFF
  INSTALL_COMMAND ${SKIP_STEP_COMMAND}
  RELATIVE_INCLUDE_PATH "src"
  DEPENDENCIES ""
  #VERBOSE
  )
