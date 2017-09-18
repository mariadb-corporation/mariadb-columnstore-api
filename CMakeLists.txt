cmake_minimum_required(VERSION 2.8)
project(mcsapi)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Provides things like lib64 dir (and Debian equiv)
include(GNUInstallDirs)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RELWITHDEBINFO CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include(cmake/version.cmake)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Disable no-deprecated-declarations
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wunused -Wwrite-strings -Wno-strict-aliasing -Wextra -Werror -Wno-deprecated-declarations")

# FORTIFY_SOURCE requires > -O0
string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASE_CMAKE_BUILD_TYPE)
if (NOT LOWERCASE_CMAKE_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2")
endif()

# Disable format-truncation since this triggers in mcsapi_types for something that isn't a problem
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wformat -Wformat-security -fstack-protector-all -fstack-check -Wno-format-truncation -Wno-unused-command-line-argument -Wno-unknown-warning-option -pie -fPIC")

INCLUDE (FindLibXml2)
if (NOT LIBXML2_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable libxml2 development environment!")
endif()

find_package(LibUV)
if (NOT LIBUV_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable libuv development environment!")
endif()

find_package(Snappy)
if (NOT SNAPPY_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable snappy development environment!")
endif()

include_directories(${CMAKE_SOURCE_DIR})

OPTION(RUN_CPPCHECK "Run cppcheck" OFF)

option(TEST_RUNNER "Build the test suite" OFF)

if (TEST_RUNNER)
  include(CTest)
  find_package(GTest REQUIRED)
  add_subdirectory(test)
endif (TEST_RUNNER)

IF(RUN_CPPCHECK)
  include(CppcheckTargets)
  if (NOT CPPCHECK_FOUND)
      MESSAGE(FATAL_ERROR "Could not find cppcheck!")
  endif()
ENDIF(RUN_CPPCHECK)

add_subdirectory(libmcsapi)
add_subdirectory(src)
add_subdirectory(example)

OPTION(PYTHON "Build Python bindings" OFF)

IF(PYTHON)
  add_subdirectory(python)
ENDIF(PYTHON)

option(BUILD_DOCS "Build the documentation" OFF)
#option(PDFLATEX_COMPILER "Build the pdf documentation (requires latex)" OFF)

if (BUILD_DOCS)
  add_subdirectory(docs)
endif (BUILD_DOCS)

include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable pkg-config development environment!")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/libmcsapi.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libmcsapi.pc" @ONLY)
install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/libmcsapi.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" COMPONENT devel)

install(FILES "README.md"
    "LICENSE"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}" COMPONENT libs
)

option(RPM "Build an RPM" OFF)

if (RPM)
    SET(CPACK_GENERATOR "RPM")
    SET(CPACK_RPM_PACKAGE_VERSION ${VERSION_SHORT})
    SET(CPACK_RPM_PACKAGE_RELEASE "1")
    SET(CPACK_RPM_PACKAGE_NAME "mariadb-columnstore-api")
    SET(ENGINE_ARCH "x86_64")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CPACK_RPM_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}-${ENGINE_ARCH}-${RPM}")

    include(CPack)
endif()

option(DEB "Build a DEB" OFF)

if (DEB)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION_SHORT})
    SET(CPACK_DEBIAN_PACKAGE_RELEASE "1")
    SET(CPACK_DEBIAN_PACKAGE_NAME "mariadb-columnstore-api")
    SET(CPACK_PACKAGE_CONTACT "MariaDB Corporation")
    SET(ENGINE_ARCH "x86_64")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}-${ENGINE_ARCH}-${DEB}")
    include(CPack)
endif()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

MESSAGE(STATUS "-----------------------------------------------")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "TEST_RUNNER = ${TEST_RUNNER}")
MESSAGE(STATUS "RUN_CPPCHECK = ${RUN_CPPCHECK}")
MESSAGE(STATUS "BUILD_DOCS = ${BUILD_DOCS}")
MESSAGE(STATUS "PYTHON = ${PYTHON}")
MESSAGE(STATUS "RPM = ${RPM}")
MESSAGE(STATUS "DEB = ${DEB}")
#MESSAGE(STATUS "PDFLATEX_COMPILER = ${PDFLATEX_COMPILER}")
MESSAGE(STATUS "Change a values with: cmake -D<Variable>=<Value>")
MESSAGE(STATUS "------------------------------------------------")
MESSAGE(STATUS)
