cmake_minimum_required(VERSION 2.8.11)

include_directories(..)
#Find the packages and libraries
IF(WIN32)
    SET(FIND_LIBRARY_USE_LIB64_PATHS ON)
    SET(ENGINE_ARCH "x64")
ENDIF(WIN32)

IF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
ENDIF(UNIX)
IF(WIN32)
    #TODO find a way to define the __FILENAME__ in Windows so that the debug output doesn't contain the absolute path.
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__=\"$(notdir $<)\"")
ENDIF(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_MYMCSAPI")

set(DOTNET_AVAILABLE 0)

execute_process(COMMAND dotnet --version OUTPUT_VARIABLE DOTNET_VERSION_AVAILABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
IF(DOTNET_VERSION_AVAILABLE VERSION_GREATER 2.0)
    set(DOTNET_AVAILABLE 1)
    MESSAGE(STATUS "dotnet ${DOTNET_VERSION_AVAILABLE} Found")
ENDIF(DOTNET_VERSION_AVAILABLE VERSION_GREATER 2.0)

IF(NOT DOTNET_AVAILABLE)
    MESSAGE(FATAL_ERROR "dotnet command line not available")
ENDIF()

#Checking for SWIG
find_package(SWIG 3 REQUIRED)

IF(DOTNET_AVAILABLE)
    MESSAGE(STATUS "Have dotnet... generate wrapper code and makefile")

    get_filename_component(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)

    set(SWIG_DOTNET_WRAPPER_SOURCE
	"${CMAKE_CURRENT_SOURCE_DIR}/dotnet_mcsapi_wrap.cxx")


    MESSAGE(STATUS "Have dotnet... Add generate wrapper code to build process ${SWIG_DOTNET_WRAPPER_SOURCE}")
    MESSAGE(STATUS "${SWIG_EXECUTABLE} -c++ -csharp -I${INCLUDE_DIR} -namespace MariaDB.Data.ColumnStore -outdir ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/dotnet_mcsapi.i")

    add_custom_command(OUTPUT ${SWIG_DOTNET_WRAPPER_SOURCE}
	    COMMAND ${SWIG_EXECUTABLE} -c++ -csharp -I${INCLUDE_DIR} -namespace MariaDB.Data.ColumnStore -outdir ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/dotnet_mcsapi.i
        COMMENT "Generating the dotnet/c# wrapper source code")

    add_library(dotnet_mcsapi SHARED "${SWIG_DOTNET_WRAPPER_SOURCE}")
    set_target_properties(dotnet_mcsapi PROPERTIES OUTPUT_NAME "dotnet_mcsapi")
    set_target_properties(dotnet_mcsapi PROPERTIES PREFIX "")
    set_target_properties(dotnet_mcsapi PROPERTIES LIBRARY_OUTPUT_DIRECTORY "glue-lib")
    IF(WIN32)
        set_target_properties(dotnet_mcsapi PROPERTIES RUNTIME_OUTPUT_DIRECTORY "glue-lib")
        target_link_libraries(dotnet_mcsapi ../lib-win-x64/mcsapi)
    ENDIF(WIN32)
    IF(UNIX)
        target_link_libraries(dotnet_mcsapi mcsapi)
        target_link_libraries(dotnet_mcsapi libmcsapi.a)
    ENDIF(UNIX)

    # Directory may not exist, so we'll copy the library in the csproj file.
    # install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/dotnet_build/dotnet_mcsapi.so" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/test/obj/Debug/netcoreapp2.1/")

ENDIF()

