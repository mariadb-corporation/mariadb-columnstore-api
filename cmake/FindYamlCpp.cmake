# Standard FIND_PACKAGE module for yaml-cpp, sets the following variables:
#   - YAML_FOUND
#   - YAML_INCLUDE_DIRS (only if YAML_FOUND)
#   - YAML_LIBRARIES (only if YAML_FOUND)

IF(UNIX)
# Try to find the header
FIND_PATH(YAML_INCLUDE_DIR NAMES yaml.h PATH_SUFFIXES yaml-cpp)

# Try to find the library
FIND_LIBRARY(YAML_LIBRARY NAMES libyaml-cpp.a yaml-cpp)
ENDIF(UNIX)

IF(WIN32)
# Try to find the header
FIND_PATH(YAML_INCLUDE_DIR NAMES yaml.h PATH_SUFFIXES include/yaml-cpp HINTS HINTS $ENV{YAML_CPP_INSTALL_DIR})

# Try to find the library
FIND_LIBRARY(YAML_LIBRARY NAMES libyaml-cppmd PATH_SUFFIXES build/RelWithDebInfo HINTS $ENV{YAML_CPP_INSTALL_DIR})
ENDIF(WIN32)

# Handle the QUIETLY/REQUIRED arguments, set YAML_FOUND if all variables are
# found
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YAML
                                  REQUIRED_VARS
                                  YAML_LIBRARY
                                  YAML_INCLUDE_DIR)

# Hide internal variables
MARK_AS_ADVANCED(YAML_INCLUDE_DIR YAML_LIBRARY)

# Set standard variables
IF(YAML_FOUND)
    IF(UNIX)
    SET(YAML_INCLUDE_DIRS "${YAML_INCLUDE_DIR}")
	ENDIF(UNIX)
	IF(WIN32)
	SET(YAML_INCLUDE_DIRS "${YAML_INCLUDE_DIR}/..")
	ENDIF(WIN32)
    SET(YAML_LIBRARIES "${YAML_LIBRARY}")
ENDIF()

