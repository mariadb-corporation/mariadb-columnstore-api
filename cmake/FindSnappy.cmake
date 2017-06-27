# Standard FIND_PACKAGE module for snappy, sets the following variables:
#   - SNAPPY_FOUND
#   - SNAPPY_INCLUDE_DIRS (only if SNAPPY_FOUND)
#   - SNAPPY_LIBRARIES (only if SNAPPY_FOUND)

# Try to find the header
FIND_PATH(SNAPPY_INCLUDE_DIR NAMES snappy.h)

# Try to find the library
FIND_LIBRARY(SNAPPY_LIBRARY NAMES snappy)

# Handle the QUIETLY/REQUIRED arguments, set LIBUV_FOUND if all variables are
# found
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SNAPPY
                                  REQUIRED_VARS
                                  SNAPPY_LIBRARY
                                  SNAPPY_INCLUDE_DIR)

# Hide internal variables
MARK_AS_ADVANCED(SNAPPY_INCLUDE_DIR SNAPPY_LIBRARY)

# Set standard variables
IF(SNAPPY_FOUND)
    SET(SNAPPY_INCLUDE_DIRS "${SNAPPY_INCLUDE_DIR}")
    SET(SNAPPY_LIBRARIES "${SNAPPY_LIBRARY}")
ENDIF()

