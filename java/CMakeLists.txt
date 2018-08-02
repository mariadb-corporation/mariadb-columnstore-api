cmake_minimum_required(VERSION 2.8)

include_directories(..)

IF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
ENDIF(UNIX)
IF(WIN32)
    #TODO find a way to define the __FILENAME__ in Windows so that the debug output doesn't contain the absolute path.
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__=\"$(notdir $<)\"")
ENDIF(WIN32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_MYMCSAPI")


find_package(SWIG REQUIRED)
find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)
include(UseSWIG)
include_directories(${JNI_INCLUDE_DIRS})

set(CMAKE_SWIG_FLAGS -package com.mariadb.columnstore.api)
set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/src/main/java/com/mariadb/columnstore/api")
set_source_files_properties( javamcsapi.i PROPERTIES CPLUSPLUS ON)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.0)
  swig_add_module(javamcsapi java javamcsapi.i)
ELSE ()

macro(SWIG_ADD_MODULE_JAVA name language)
  string(TOUPPER "${language}" swig_uppercase_language)
  string(TOLOWER "${language}" swig_lowercase_language)
  set(SWIG_MODULE_${name}_LANGUAGE "${swig_uppercase_language}")
  set(SWIG_MODULE_${name}_SWIG_LANGUAGE_FLAG "${swig_lowercase_language}")

  set(SWIG_MODULE_${name}_REAL_NAME "${name}")
  set(swig_dot_i_sources)
  set(swig_other_sources)
  foreach(it ${ARGN})
    if(${it} MATCHES ".*\\.i$")
      set(swig_dot_i_sources ${swig_dot_i_sources} "${it}")
    else()
      set(swig_other_sources ${swig_other_sources} "${it}")
    endif()
  endforeach()

  set(swig_generated_sources)
  foreach(it ${swig_dot_i_sources})
    SWIG_ADD_SOURCE_TO_MODULE(${name} swig_generated_source ${it})
    set(swig_generated_sources ${swig_generated_sources} "${swig_generated_source}")
  endforeach()
  get_directory_property(swig_extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)
  set_directory_properties(PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${swig_extra_clean_files};${swig_generated_sources}")
  add_library(${SWIG_MODULE_${name}_REAL_NAME}
    MODULE
    ${swig_generated_sources}
    ${swig_other_sources})
  string(TOLOWER "${language}" swig_lowercase_language)
  if ("${swig_lowercase_language}" STREQUAL "java")
    if (APPLE)
        # In java you want:
        #      System.loadLibrary("LIBRARY");
        # then JNI will look for a library whose name is platform dependent, namely
        #   MacOS  : libLIBRARY.jnilib
        #   Windows: LIBRARY.dll
        #   Linux  : libLIBRARY.so
        set_target_properties (${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES SUFFIX ".jnilib")
      endif ()
  endif ()
  set_target_properties(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES PREFIX "lib")
endmacro()
swig_add_module_java(javamcsapi java javamcsapi.i)
ENDIF ()
swig_link_libraries(javamcsapi mcsapi)

IF(UNIX)
    set_target_properties(javamcsapi PROPERTIES 
        SUFFIX ".so.${MCSAPI_VERSION_STRING}"
    )

    ADD_CUSTOM_COMMAND(TARGET javamcsapi POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} jar
        COMMAND ${CMAKE_COMMAND} -E create_symlink javamcsapi-${MCSAPI_VERSION_STRING}.jar "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/javamcsapi.jar"
        COMMAND ${CMAKE_COMMAND} -E create_symlink libjavamcsapi.so.${MCSAPI_VERSION_STRING} "${CMAKE_CURRENT_BINARY_DIR}/libjavamcsapi.so"
    )
ENDIF(UNIX)
IF(WIN32)
    ADD_CUSTOM_COMMAND(TARGET javamcsapi POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew.bat" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} jar
    )
ENDIF(WIN32)

IF(UNIX)
    INSTALL(TARGETS javamcsapi DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libjavamcsapi.so" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
    INSTALL(FILES build/libs/javamcsapi-${MCSAPI_VERSION_STRING}.jar DESTINATION  ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
    INSTALL(FILES build/libs/javamcsapi.jar DESTINATION  ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
ENDIF(UNIX)
IF(WIN32)
    INSTALL(TARGETS javamcsapi DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT libs)
    INSTALL(FILES build/libs/javamcsapi-${MCSAPI_VERSION_STRING}.jar DESTINATION  "${CMAKE_INSTALL_LIBDIR}/java" COMPONENT libs)
ENDIF(WIN32)

if (BUILD_DOCS)
    add_subdirectory(docs)
endif (BUILD_DOCS)

IF(TEST_RUNNER)
    option(JAVA_COMPATIBILITY_TEST "Run the Java mcsapi compatibility test" ON)
    IF(UNIX)
        add_test(NAME Java_BasicTest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} -Pjava.library.path=${CMAKE_CURRENT_BINARY_DIR} test)
        IF(JAVA_COMPATIBILITY_TEST)
            add_test(NAME JavaMcsapi_mcsapi_compatibility_test COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/test/compatibility_test.sh" ${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH} ${CMAKE_CURRENT_BINARY_DIR})
        ENDIF(JAVA_COMPATIBILITY_TEST)
    ENDIF(UNIX)
    IF(WIN32)
        add_test(NAME Java_BasicTest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew.bat" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} -Pjava.library.path=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION> test)
        IF(JAVA_COMPATIBILITY_TEST)
            #TODO transform compatibility_test.sh into a batch or PowerShell script
        ENDIF(JAVA_COMPATIBILITY_TEST)

        # On Windows copy the required shared libraries to execute the Java tests as well.
        ADD_CUSTOM_TARGET(copy_required_shared_java_test_libraries ALL
            COMMAND ${CMAKE_COMMAND} -E copy ${LIBXML2_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            COMMAND ${CMAKE_COMMAND} -E copy ${LIBICONV_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            COMMAND ${CMAKE_COMMAND} -E copy ${LIBUV_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            COMMAND ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:mcsapi> "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
            DEPENDS mcsapi javamcsapi
            COMMENT "Copy the shared libraries for executing the Java tests"
        )
    ENDIF(WIN32)
ENDIF(TEST_RUNNER)
