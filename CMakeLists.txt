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

CHECK_CXX_SOURCE_COMPILES("
#include <time.h>
#include <sstream>
#include <iomanip>
int main(int argc, char** argv){
    tm time = {};
    std::istringstream ss(\"01-01-01 00:00:00\");
    ss >> std::get_time(&time, \"%Y\");
    return 0;
}" HAVE_GET_TIME)

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
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wunused -Wwrite-strings -Wno-strict-aliasing -Wextra -Werror -Wno-deprecated-declarations")
endif(CMAKE_COMPILER_IS_GNUCXX)
    
# FORTIFY_SOURCE requires > -O0
string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASE_CMAKE_BUILD_TYPE)
if (NOT LOWERCASE_CMAKE_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2")
endif()

# Disable format-truncation since this triggers in mcsapi_types for something that isn't a problem
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wformat-security -fstack-protector-all -fstack-check -Wno-format-truncation -Wno-unused-command-line-argument -Wno-unknown-warning-option -pie -fPIC")
endif(CMAKE_COMPILER_IS_GNUCXX)

#Find the packages and libraries
IF(WIN32)
    SET(FIND_LIBRARY_USE_LIB64_PATHS ON)
    SET(CMAKE_FIND_LIBRARY_PREFIXES_BAK "${CMAKE_FIND_LIBRARY_PREFIXES}")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES_BAK "${CMAKE_FIND_LIBRARY_SUFFIXES}")
ENDIF(WIN32)

INCLUDE(FindLibXml2)
if (NOT LIBXML2_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable libxml2 development environment!")
endif()
IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ".DLL")

    find_library(
    LIBXML2_RUNTIME_LIBRARY
    NAMES LIBXML2 libxml2
    HINTS $ENV{LIBXML2_RUNTIME_DIRECTORY}
    )
    if (${LIBXML2_RUNTIME_LIBRARY} STREQUAL "LIBXML2_RUNTIME_LIBRARY-NOTFOUND")
        MESSAGE(FATAL_ERROR "Could not find a usable LibXml2 runtime library libxml2.dll!\nYou can hint cmake by setting the environment variable LIBXML2_RUNTIME_DIRECTORY.")
    ENDIF()
    MESSAGE(STATUS "Found LibXml2 runtime library: ${LIBXML2_RUNTIME_LIBRARY}")

    find_library(
        LIBICONV_RUNTIME_LIBRARY
        NAMES LIBICONV libiconv
        HINTS $ENV{LIBICONV_RUNTIME_DIRECTORY}
    )
    if (${LIBICONV_RUNTIME_LIBRARY} STREQUAL "LIBICONV_RUNTIME_LIBRARY-NOTFOUND")
        MESSAGE(FATAL_ERROR "Could not find a usable libiconv runtime library libiconv.dll!\nYou can hint cmake by setting the environment variable LIBICONV_RUNTIME_DIRECTORY.")
    ENDIF()
    MESSAGE(STATUS "Found libiconv runtime library: ${LIBICONV_RUNTIME_LIBRARY}")
    
    SET(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES_BAK}")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES_BAK}")
ENDIF(WIN32)

find_package(LibUV)
if (NOT LIBUV_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable libuv development environment!")
endif()
IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ".DLL")
    
    find_library(
        LIBUV_RUNTIME_LIBRARY
        NAMES LIBUV libuv
        HINTS $ENV{LIBUV_RUNTIME_DIRECTORY}
    )
    if (${LIBUV_RUNTIME_LIBRARY} STREQUAL "LIBUV_RUNTIME_LIBRARY-NOTFOUND")
        MESSAGE(FATAL_ERROR "Could not find a usable LIBUV runtime library libuv.dll!\nYou can hint cmake by setting the environment variable LIBUV_RUNTIME_DIRECTORY.")
    ENDIF()
    MESSAGE(STATUS "Found LIBUV runtime library: ${LIBUV_RUNTIME_LIBRARY}")
    
    SET(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES_BAK}")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES_BAK}")
ENDIF(WIN32)

find_package(Snappy)
if (NOT SNAPPY_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable snappy development environment!")
endif()

find_package(Boost REQUIRED)

if (Boost_VERSION VERSION_LESS 105800)
    find_path(BOOST_SUB_DIR "boost/multiprecision/include")

    if ((NOT BOOST_SUB_DIR) OR (NOT EXISTS ${BOOST_SUB_DIR}))
        execute_process(COMMAND git submodule update --init
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

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

option(DEB "Build a DEB" OFF)
option(RPM "Build an RPM" OFF)
OPTION(PYTHON "Build Python bindings" ON)

IF(PYTHON)
  add_subdirectory(python)
ENDIF(PYTHON)

OPTION(JAVA "Build Java bindings" ON)

IF(JAVA)
    add_subdirectory(java)
ENDIF(JAVA)

OPTION(SPARK_CONNECTOR "Build spark connector" ON)

IF(SPARK_CONNECTOR)
  IF(JAVA)
    add_subdirectory(spark-connector/scala)
  ENDIF(JAVA)
  IF(PYTHON)
    add_subdirectory(spark-connector/python)
  ENDIF(PYTHON)
ENDIF(SPARK_CONNECTOR)  

OPTION(SIGNCODE "Sign the MSI installer" OFF)

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
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libmcsapi.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" COMPONENT devel)

install(FILES "README.md"
    "LICENSE.txt"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}" COMPONENT libs
)

if (RPM)
    SET(CPACK_GENERATOR "RPM")
    SET(CPACK_RPM_PACKAGE_VERSION ${VERSION_SHORT})
    SET(CPACK_RPM_PACKAGE_RELEASE "1")
    SET(CPACK_RPM_PACKAGE_NAME "mariadb-columnstore-api")
    SET(ENGINE_ARCH "x86_64")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CPACK_RPM_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}-${ENGINE_ARCH}-${RPM}")
    include(CPack)
endif()

if (DEB)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION_SHORT})
    SET(CPACK_DEBIAN_PACKAGE_RELEASE "1")
    SET(CPACK_DEBIAN_PACKAGE_NAME "mariadb-columnstore-api")
    SET(CPACK_PACKAGE_CONTACT "MariaDB Corporation")
    SET(ENGINE_ARCH "amd64")
    SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}-${DEB}.${ENGINE_ARCH}")
    include(CPack)
endif()

if (WIN32)
    SET(CPACK_GENERATOR "WIX")
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    SET(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/resources/WixUIBannerBmp.jpg")
    SET(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/resources/WixUIDialogBmp.jpg")
    SET(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/mariadb.ico")
    SET(CPACK_PACKAGE_NAME "MariaDB ColumnStore Bulk Write SDK")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MariaDB ColumnStore Bulk Write SDK")
    set(CPACK_PACKAGE_VENDOR "MariaDB Corporation Ab")
    SET(CPACK_PACKAGE_CONTACT "MariaDB Corporation Ab")
    SET(ENGINE_ARCH "x64")
    SET(CPACK_PACKAGE_VERSION ${VERSION_SHORT})
    set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
    set(CPACK_PACKAGE_VERSION_RELEASE "1")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_VERSION_RELEASE}-${ENGINE_ARCH}")
    set(CPACK_PACKAGE_FILE_NAME_COPY "${CPACK_PACKAGE_FILE_NAME}")
    set(CPACK_WIX_UPGRADE_GUID "CDE3BE0F-EAFE-417C-A8CF-4A5F14451845")
    set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY ON)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "MariaDB/ColumnStore Bulk Write SDK")
    set(CPACK_COMPONENT_LIBS_DISPLAY_NAME "libraries")
    set(CPACK_COMPONENT_LIBS_DESCRIPTION "Installs the C++, Python, Java, and Spark connector libraries")
    set(CPACK_COMPONENT_DEVEL_DISPLAY_NAME "development")
    set(CPACK_COMPONENT_DEVEL_DESCRIPTION "Installs the C++ header files and examples")
    if(SIGNCODE)
        SET( CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/WinPackageConfig.cmake" )
        include(CPack)
        ADD_CUSTOM_TARGET ( package
            COMMAND "${CMAKE_CPACK_COMMAND}" 
                "-C" "$<CONFIGURATION>"
                "--config" "${CMAKE_BINARY_DIR}/WinPackageConfig.cmake"
            COMMENT "-- Running CPack..."
        )
        add_dependencies(package mcsapi mcsapi_static basic_bulk_insert advanced_bulk_insert)
        IF(JAVA)
            add_dependencies(package javamcsapi)
        ENDIF()
        IF(PYTHON)
            IF(${PYTHON2_AVAILABLE})
                add_dependencies(package _pymcsapi)
            ENDIF()
            IF(${PYTHON3_AVAILABLE})
                add_dependencies(package _pymcsapi3)
            ENDIF()
        ENDIF()
        IF(SPARK_CONNECTOR)
            IF(JAVA)
                add_dependencies(package spark-scala-mcsapi-connector)
            ENDIF()
            IF(PYTHON)
                #nothing to do as this are static files only
            ENDIF()
        ENDIF()
        # sign the msi installer
        # https://www.digicert.com/code-signing/signcode-signtool-command-line.htm
        add_custom_command(
            TARGET package
            POST_BUILD
            COMMAND signtool.exe sign 
            /tr http://timestamp.digicert.com 
            /td sha256 
            /fd sha256 
            /a "${CPACK_PACKAGE_FILE_NAME_COPY}.msi" 
            COMMENT "-- Signing package..."
        )
    else(SIGNCODE)
        include(CPack)
    endif(SIGNCODE)
endif(WIN32)

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
MESSAGE(STATUS "JAVA = ${JAVA}")
MESSAGE(STATUS "SPARK_CONNECTOR = ${SPARK_CONNECTOR}")
MESSAGE(STATUS "RPM = ${RPM}")
MESSAGE(STATUS "DEB = ${DEB}")
#MESSAGE(STATUS "PDFLATEX_COMPILER = ${PDFLATEX_COMPILER}")
MESSAGE(STATUS "Change a values with: cmake -D<Variable>=<Value>")
MESSAGE(STATUS "------------------------------------------------")
MESSAGE(STATUS)
