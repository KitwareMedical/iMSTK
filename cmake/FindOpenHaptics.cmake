find_path(OPENHAPTICS_INCLUDE_DIR
  HD/hd.h)
find_library(OPENHAPTICS_hd_LIBRARY
  NAMES
    hd)
find_library(OPENHAPTICS_hdu_LIBRARY
  NAMES
    hdu)

set(OPENHAPTICS_INCLUDE_DIRS "${OPENHAPTICS_INCLUDE_DIR}")
set(OPENHAPTICS_LIBRARIES "${OPENHAPTICS_hd_LIBRARY}" "${OPENHAPTICS_hdu_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPENHAPTICS
  REQUIRED_VARS
    OPENHAPTICS_INCLUDE_DIR
    OPENHAPTICS_hd_LIBRARY
    OPENHAPTICS_hdu_LIBRARY)

mark_as_advanced(
  OPENHAPTICS_INCLUDE_DIR
  OPENHAPTICS_hd_LIBRARY
  OPENHAPTICS_hdu_LIBRARY)