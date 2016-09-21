#-----------------------------------------------------------------------------
# Find path
#-----------------------------------------------------------------------------
find_path(SCCD_INCLUDE_DIR
  NAMES
    ccdAPI.h
    )
mark_as_advanced(SCCD_INCLUDE_DIR)
#message(STATUS "SCCD_INCLUDE_DIR : ${SCCD_INCLUDE_DIR}")

#-----------------------------------------------------------------------------
# Find library
#-----------------------------------------------------------------------------
find_library(SCCD_LIBRARY
  NAMES
    libsccd
    sccd
  )
mark_as_advanced(SCCD_LIBRARY)
#message(STATUS "SCCD_LIBRARY : ${SCCD_LIBRARY}")

#-----------------------------------------------------------------------------
# Find package
#-----------------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SCCD
  REQUIRED_VARS
    SCCD_INCLUDE_DIR
    SCCD_LIBRARY)

#-----------------------------------------------------------------------------
# If missing target, create it
#-----------------------------------------------------------------------------
if(SCCD_FOUND AND NOT TARGET SCCD)
  add_library(SCCD INTERFACE IMPORTED)
  set_target_properties(SCCD PROPERTIES
    INTERFACE_LINK_LIBRARIES "${SCCD_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SCCD_INCLUDE_DIR}"
  )
endif()