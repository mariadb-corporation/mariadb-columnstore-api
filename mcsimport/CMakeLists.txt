cmake_minimum_required(VERSION 2.8)
project(mcsimport)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Provides things like lib64 dir (and Debian equiv)
include(GNUInstallDirs)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no GNU++11 support. Please use a different C++ compiler.")
endif()

include_directories(${CMAKE_SOURCE_DIR}/libmcsapi)

IF(UNIX)
  INCLUDE(FindYamlCpp)
ENDIF(UNIX)
IF(WIN32)
  INCLUDE(FindYamlCpp)
ENDIF(WIN32)

IF (NOT YAML_FOUND)
  IF(UNIX)
    MESSAGE(FATAL_ERROR "Could not find libyaml-cpp")
  ENDIF(UNIX)
  IF(WIN32)
    MESSAGE(FATAL_ERROR "Could not find libyaml-cppmd.\nYou can hint cmake by setting the environment variable YAML_CPP_INSTALL_DIR")
  ENDIF(WIN32)
ENDIF()
include_directories(${YAML_INCLUDE_DIRS})

IF(UNIX)
  # The flags inherited from the top-level cmakelists currently break
  # linking to static libs.  Need to revisit that at some point.
  set(CMAKE_CXX_FLAGS "-std=gnu++11 -O3")
ENDIF(UNIX)

SET(MCSIMPORT_FILES
    mcsimport.cpp
)

add_executable(mcsimport ${MCSIMPORT_FILES})

target_link_libraries(mcsimport mcsapi ${YAML_LIBRARIES})

IF(WIN32)
    INSTALL(TARGETS mcsimport RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT mcsimport)
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION "${CMAKE_INSTALL_DOCDIR}" COMPONENT mcsimport)
ELSE(WIN32)
    INSTALL(TARGETS mcsimport RUNTIME DESTINATION "${BINDIR}" COMPONENT mcsimport)
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION "${SUPPORTDIR}/mcsimport" COMPONENT mcsimport)
ENDIF(WIN32)

IF(TEST_RUNNER)
    execute_process(COMMAND python -c "import sys; print(sys.version_info[0]);" OUTPUT_VARIABLE PYTHON_VERSION_AVAILABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
    IF(PYTHON_VERSION_AVAILABLE EQUAL 2)
        execute_process(COMMAND python -c "import sys; print(sys.executable);" OUTPUT_VARIABLE PYTHON2_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
    ELSE(PYTHON_VERSION_AVAILABLE EQUAL 2)
         MESSAGE(FATAL_ERROR "Python2 is required to test mcsapi")
    ENDIF(PYTHON_VERSION_AVAILABLE EQUAL 2)
    # On Windows copy the required shared libraries to execute the tests.
    IF(WIN32)
        ADD_CUSTOM_TARGET(copy_required_shared_test_libraries ALL
            COMMAND ${CMAKE_COMMAND} -E copy ${MCSAPI_LIBXML2_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            COMMAND ${CMAKE_COMMAND} -E copy ${MCSAPI_LIBICONV_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            COMMAND ${CMAKE_COMMAND} -E copy ${MCSAPI_LIBUV_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            COMMAND ${CMAKE_COMMAND} -E copy ${MCSAPI_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            DEPENDS mcsimport
            COMMENT "Copy the shared libraries for executing the tests"
        )
        add_test(NAME mcsimport_test_suite COMMAND "${PYTHON2_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test/test.py" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/mcsimport.exe")
    ENDIF(WIN32)
    IF(UNIX)
        add_test(NAME mcsimport_test_suite COMMAND "${PYTHON2_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test/test.py" "${CMAKE_CURRENT_BINARY_DIR}/mcsimport")
    ENDIF(UNIX)
    set_tests_properties(mcsimport_test_suite PROPERTIES TIMEOUT 3600)
ENDIF(TEST_RUNNER)

# Install the mcsapi DLLs with the mcsimport binary
IF(WIN32)
    INSTALL(FILES ${MCSAPI_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT mcsimport)
    INSTALL(FILES ${MCSAPI_LIBXML2_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT mcsimport)
    INSTALL(FILES ${MCSAPI_LIBICONV_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT mcsimport)
    INSTALL(FILES ${MCSAPI_LIBUV_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT mcsimport)
ENDIF(WIN32)

