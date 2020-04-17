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
include(cmake/parentdirlist.cmake)

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

# Find the packages and libraries
IF(WIN32)
    SET(FIND_LIBRARY_USE_LIB64_PATHS ON)
    SET(CMAKE_FIND_LIBRARY_PREFIXES_BAK "${CMAKE_FIND_LIBRARY_PREFIXES}")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES_BAK "${CMAKE_FIND_LIBRARY_SUFFIXES}")
    # Include external lib and include directory if environment variable MCSAPI_BUILD_DEPENDENCY_DIR is set
    IF(DEFINED ENV{MCSAPI_BUILD_DEPENDENCY_DIR})
        LIST(APPEND CMAKE_PREFIX_PATH $ENV{MCSAPI_BUILD_DEPENDENCY_DIR})
        MESSAGE(STATUS "Added $ENV{MCSAPI_BUILD_DEPENDENCY_DIR} to CMAKE_PREFIX_PATH")
    ELSE()
        MESSAGE(STATUS "Notice: Set the environment variable MCSAPI_BUILD_DEPENDENCY_DIR to define an additional dependency directory to search for headers and libraries.")
    ENDIF(DEFINED ENV{MCSAPI_BUILD_DEPENDENCY_DIR})
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

# On Windows find the Microsoft Visual Studio 2017 C++ Redistributable merge module (x64) that is packaged with mcsapi
IF(WIN32)
    # Macro to get the subdirectories of a given directory
    MACRO(SUBDIRLIST result curdir)
      FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
      SET(dirlist "")
      FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
          LIST(APPEND dirlist ${child})
        ENDIF()
      ENDFOREACH()
      SET(${result} ${dirlist})
    ENDMACRO()

    # Build up potential merge module paths
    SET(VISUAL_STUDIO_PATHS)
    LIST(APPEND VISUAL_STUDIO_PATHS "$ENV{ProgramFiles}/Microsoft Visual Studio/2017/Community/VC/Redist/MSVC")
    LIST(APPEND VISUAL_STUDIO_PATHS "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio/2017/Community/VC/Redist/MSVC")
    LIST(APPEND VISUAL_STUDIO_PATHS "$ENV{ProgramFiles}/Microsoft Visual Studio/2017/Enterprise/VC/Redist/MSVC")
    LIST(APPEND VISUAL_STUDIO_PATHS "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Redist/MSVC")
    LIST(APPEND VISUAL_STUDIO_PATHS "$ENV{ProgramFiles}/Microsoft Visual Studio/2017/Professional/VC/Redist/MSVC")
    LIST(APPEND VISUAL_STUDIO_PATHS "$ENV{ProgramFiles} (x86)/Microsoft Visual Studio/2017/Professional/VC/Redist/MSVC")
    
    SET(MSM_PATHS)
    FOREACH(VISUAL_STUDIO_PATH ${VISUAL_STUDIO_PATHS})
        file(TO_CMAKE_PATH ${VISUAL_STUDIO_PATH} VISUAL_STUDIO_PATH_CONV)
        SUBDIRLIST(SUB_DIRS ${VISUAL_STUDIO_PATH_CONV})
        FOREACH(SUB_DIR ${SUB_DIRS})
            LIST(APPEND MSM_PATHS "${VISUAL_STUDIO_PATH}/${SUB_DIR}/MergeModules")
        ENDFOREACH()
    ENDFOREACH()
    
    # Try to find the merge module
    find_file(
        VS_2017_REDIST_MSM
        NAMES Microsoft_VC141_CRT_x64.msm Microsoft_VC140_CRT_x64.msm
        HINTS $ENV{MSM_DIRECTORY}
        PATHS ${MSM_PATHS}
    )
    if (${VS_2017_REDIST_MSM} STREQUAL "VS_2017_REDIST_MSM-NOTFOUND")
        MESSAGE(FATAL_ERROR "Could not find a usable Visual Studio 2015/17 C++ Redistributable merge module Microsoft_VC141_CRT_x64.msm!\nYou can hint cmake by setting the environment variable MSM_DIRECTORY.")
    ENDIF()
    MESSAGE(STATUS "Found Visual Studio 2015/17 C++ Redistributable merge module: ${VS_2017_REDIST_MSM}")
ENDIF(WIN32)

IF (RPM)
    SET (BINDIR     "/usr/bin")
    SET (SUPPORTDIR "/usr/share/columnstore")
ELSEIF (DEB)
    SET (BINDIR     "/usr/bin")
    SET (SUPPORTDIR "/usr/share/columnstore")
ELSEIF (WIN32)
    SET (BINDIR     ${CMAKE_INSTALL_BINDIR})
    SET (SUPPORTDIR ${CMAKE_INSTALL_DOCDIR})
ELSE()
    SET (BINDIR     "/usr/local/bin")
    SET (SUPPORTDIR "/usr/local/share/columnstore")
ENDIF()

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


option(REMOTE_CPIMPORT "Build the mcsimport tool" ON)
IF(REMOTE_CPIMPORT)
    add_subdirectory(mcsimport)
ENDIF(REMOTE_CPIMPORT)

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
IF(WIN32)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libmcsapi.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" COMPONENT cpp_devel) #Windows requires underscore
ELSE(WIN32)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libmcsapi.pc"
       DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" COMPONENT cpp-devel) #Debian requires dash
    
    # for some reason on centos 7, this gets packaged under /usr/lib64/pkgconfig, but 
    # ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} is /usr/local/lib64/.  Hardcoding it for now.
    list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/lib64/pkgconfig)
ENDIF(WIN32)

install(FILES "README.md"
    "LICENSE.txt"
    DESTINATION "${SUPPORTDIR}/mcsapi" COMPONENT cpp
)
IF(WIN32)
    install(FILES "resources/3rd party licenses.txt"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}" COMPONENT cpp
    )
ENDIF(WIN32)

if (RPM)
    SET(CPACK_GENERATOR "RPM")
    SET(CPACK_RPM_PACKAGE_VERSION ${VERSION_SHORT})
    SET(CPACK_RPM_PACKAGE_RELEASE "1")
    SET(CPACK_RPM_PACKAGE_NAME "MariaDB-columnstore-api")
    SET(CPACK_RPM_PACKAGE_SUMMARY "MariaDB ColumnStore API: A SDK to rapidly ingest data into MariaDB ColumnStore")
    SET(CPACK_RPM_PACKAGE_LICENSE "GPLv2")
    SET(ENGINE_ARCH "x86_64")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CPACK_RPM_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}-${ENGINE_ARCH}-${RPM}")
    SET(CPACK_RPM_COMPONENT_INSTALL ON)
    SET(CPACK_RPM_cpp_PACKAGE_REQUIRES "libuv, libxml2, snappy")
    SET(CPACK_RPM_cpp-devel_PACKAGE_REQUIRES "MariaDB-columnstore-api-cpp == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_java_PACKAGE_REQUIRES "MariaDB-columnstore-api-cpp == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_python_PACKAGE_REQUIRES "MariaDB-columnstore-api-cpp == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_python3_PACKAGE_REQUIRES "MariaDB-columnstore-api-cpp == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_spark_PACKAGE_REQUIRES "MariaDB-columnstore-api-java == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_pyspark_PACKAGE_REQUIRES "MariaDB-columnstore-api-python == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_pyspark3_PACKAGE_REQUIRES "MariaDB-columnstore-api-python3 == ${CPACK_RPM_PACKAGE_VERSION}")
    SET(CPACK_RPM_mcsimport_PACKAGE_REQUIRES "MariaDB-columnstore-api-cpp == ${CPACK_RPM_PACKAGE_VERSION}")
    include(CPack)
endif()

if (DEB)
    SET(CPACK_DEBIAN_PACKAGE_DEBUG 1)
    SET(CPACK_PACKAGE_VERSION_MAJOR ${MCSAPI_VERSION_MAJOR})
    SET(CPACK_PACKAGE_VERSION_MINOR ${MCSAPI_VERSION_MINOR})
    SET(CPACK_PACKAGE_VERSION_PATCH ${MCSAPI_VERSION_PATCH})
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION_SHORT})
    SET(PACKAGE_RELEASE "1") #need to change due to changed behaviour in CMake > 3.10
    SET(CPACK_DEBIAN_PACKAGE_NAME "MariaDB-columnstore-api")
    SET(CPACK_DEBIAN_PACKAGE_SUMMARY "MariaDB ColumnStore API: A SDK to rapidly ingest data into MariaDB ColumnStore")
    SET(CPACK_PACKAGE_CONTACT "MariaDB Corporation")
    SET(ENGINE_ARCH "amd64")
    SET(CPACK_DEBIAN_PACKAGE_LICENSE "GPLv2")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}-${PACKAGE_RELEASE}-${DEB}.${ENGINE_ARCH}")
    SET(CPACK_DEB_COMPONENT_INSTALL ON)
    SET(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
    SET(CPACK_DEBIAN_CPP-DEVEL_PACKAGE_DEPENDS "mariadb-columnstore-api-cpp (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_JAVA_PACKAGE_DEPENDS "mariadb-columnstore-api-cpp (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_PYTHON_PACKAGE_DEPENDS "mariadb-columnstore-api-cpp (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_PYTHON3_PACKAGE_DEPENDS "mariadb-columnstore-api-cpp (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_SPARK_PACKAGE_DEPENDS "mariadb-columnstore-api-java (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_PYSPARK_PACKAGE_DEPENDS "mariadb-columnstore-api-python (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_PYSPARK3_PACKAGE_DEPENDS "mariadb-columnstore-api-python3 (= ${VERSION_SHORT})")
    SET(CPACK_DEBIAN_MCSIMPORT_PACKAGE_DEPENDS "mariadb-columnstore-api-cpp (= ${VERSION_SHORT})")
    include(CPack)
endif()

if (WIN32)
    SET(CPACK_GENERATOR "WIX")
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    SET(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/resources/WixUIBannerBmp.jpg")
    SET(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/resources/WixUIDialogBmp.jpg")
    SET(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/mariadb.ico")
    SET(WIX_ADDITIONAL_FILES)
    IF(PYTHON2_AVAILABLE)
        # First variable substitution of @VARIABLE@ during configure time
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python2.wxs.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python2.wxs.cf"
            IMMEDIATE @ONLY)
        # Second variable substitution of generate_expressions during generate time
        file(GENERATE
            OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python2.wxs"
            INPUT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python2.wxs.cf"
            CONDITION $<CONFIG:${CMAKE_BUILD_TYPE}>
        )
        LIST(APPEND WIX_ADDITIONAL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python2.wxs")
        # Set the variable to include defined feature to be substitued later
        SET(PYTHON_2_FEATRUE_REF "<FeatureRef Id='Python2DirectInstallation'/>")
    ENDIF(PYTHON2_AVAILABLE)
    IF(PYTHON3_AVAILABLE)
        # First variable substitution of @VARIABLE@ during configure time
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python3.wxs.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python3.wxs.cf"
            IMMEDIATE @ONLY)
        # Second variable substitution of generate_expressions during generate time
        file(GENERATE
            OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python3.wxs"
            INPUT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python3.wxs.cf"
            CONDITION $<CONFIG:${CMAKE_BUILD_TYPE}>
        )
        LIST(APPEND WIX_ADDITIONAL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/python3.wxs")
        # Set the variable to include defined feature to be substitued later
        SET(PYTHON_3_FEATRUE_REF "<FeatureRef Id='Python3DirectInstallation'/>")
    ENDIF(PYTHON3_AVAILABLE)
    SET(CPACK_WIX_EXTRA_SOURCES ${WIX_ADDITIONAL_FILES})
    SET(WIX_PATCH_FILES)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/product_fragment.patch.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/product_fragment.patch"
        IMMEDIATE @ONLY)
    LIST(APPEND WIX_PATCH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/product_fragment.patch")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/features.patch.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/features.patch"
        IMMEDIATE @ONLY)
    LIST(APPEND WIX_PATCH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpack_windows/features.patch")
    SET(CPACK_WIX_PATCH_FILE ${WIX_PATCH_FILES})
    SET(CPACK_PACKAGE_NAME "MariaDB ColumnStore Bulk Write SDK and Remote Import Tool")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MariaDB ColumnStore Bulk Write SDK and Remote Import Tool")
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
	set(CPACK_COMPONENT_CPP_REQUIRED 1)
	set(CPACK_COMPONENT_CPP_DEVEL_DEPENDS cpp)
	set(CPACK_COMPONENT_JAVA_DEPENDS cpp)
	set(CPACK_COMPONENT_PYTHON_DEPENDS cpp)
	set(CPACK_COMPONENT_PYTHON3_DEPENDS cpp)
	set(CPACK_COMPONENT_SPARK_DEPENDS java)
	set(CPACK_COMPONENT_PYSPARK_DEPENDS python)
	set(CPACK_COMPONENT_PYSPARK3_DEPENDS python3)
    set(CPACK_COMPONENT_CPP_DISPLAY_NAME "C++ libraries")
    set(CPACK_COMPONENT_CPP_DESCRIPTION "Installs the C++ shared libraries")
    set(CPACK_COMPONENT_CPP_DEVEL_DISPLAY_NAME "C++ development")
    set(CPACK_COMPONENT_CPP_DEVEL_DESCRIPTION "Installs the C++ header files for development and examples")
	set(CPACK_COMPONENT_JAVA_DISPLAY_NAME "Java libraries")
    set(CPACK_COMPONENT_JAVA_DESCRIPTION "Installs the Java libraries for usage and development")
	set(CPACK_COMPONENT_PYTHON_DISPLAY_NAME "Python libraries")
    set(CPACK_COMPONENT_PYTHON_DESCRIPTION "Installs Python libraries for usage and development")
	set(CPACK_COMPONENT_PYTHON3_DISPLAY_NAME "Python3 libraries")
    set(CPACK_COMPONENT_PYTHON3_DESCRIPTION "Installs the Python3 libraries for usage and libraries")
	set(CPACK_COMPONENT_SPARK_DISPLAY_NAME "Spark exporter")
    set(CPACK_COMPONENT_SPARK_DESCRIPTION "Installs the ColumnStoreExporter for usage with Spark and Scala")
	set(CPACK_COMPONENT_PYSPARK_DISPLAY_NAME "PySpark exporter")
    set(CPACK_COMPONENT_PYSPARK_DESCRIPTION "Installs the ColumnStoreExporter for usage with PySpark and Python 2.7")
	set(CPACK_COMPONENT_PYSPARK3_DISPLAY_NAME "PySpark3 exporter")
    set(CPACK_COMPONENT_PYSPARK3_DESCRIPTION "Installs the ColumnStoreExporter for usage with PySpark and Python 3")

    IF(REMOTE_CPIMPORT)
        SET(CPACK_COMPONENT_MCSIMPORT_DISPLAY_NAME "MCS Import")
        set(CPACK_COMPONENT_MCSIMPORT_DESCRIPTION "MariaDB ColumnStore Remote Import Tool")
        set(CPACK_COMPONENT_MCSIMPORT_INSTALL_DIRECTORY "MariaDB/ColumnStore Remote Import")
        set(CPACK_COMPONENT_MCSIMPORT_DEPENDS cpp)
    ENDIF(REMOTE_CPIMPORT)
    include(CPack)
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
MESSAGE(STATUS "REMOTE_CPIMPORT = ${REMOTE_CPIMPORT}")
#MESSAGE(STATUS "PDFLATEX_COMPILER = ${PDFLATEX_COMPILER}")
MESSAGE(STATUS "Change a values with: cmake -D<Variable>=<Value>")
MESSAGE(STATUS "------------------------------------------------")
MESSAGE(STATUS)


