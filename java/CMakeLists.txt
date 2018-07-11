cmake_minimum_required(VERSION 2.8)

include_directories(..)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

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

set_target_properties(javamcsapi PROPERTIES 
    SUFFIX ".so.${MCSAPI_VERSION_STRING}"
)

ADD_CUSTOM_COMMAND(TARGET javamcsapi POST_BUILD
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} jar
    COMMAND ${CMAKE_COMMAND} -E create_symlink javamcsapi-${MCSAPI_VERSION_STRING}.jar "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/javamcsapi.jar"
    COMMAND ${CMAKE_COMMAND} -E create_symlink libjavamcsapi.so.${MCSAPI_VERSION_STRING} "${CMAKE_CURRENT_BINARY_DIR}/libjavamcsapi.so"
)

INSTALL(TARGETS javamcsapi DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libjavamcsapi.so" DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL(FILES build/libs/javamcsapi-${MCSAPI_VERSION_STRING}.jar DESTINATION  ${CMAKE_INSTALL_LIBDIR})
INSTALL(FILES build/libs/javamcsapi.jar DESTINATION  ${CMAKE_INSTALL_LIBDIR})

if (BUILD_DOCS)
    add_subdirectory(docs)
endif (BUILD_DOCS)

IF(TEST_RUNNER)
    add_test(NAME Java_BasicTest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} -Pjava.library.path=${CMAKE_CURRENT_BINARY_DIR} test)
ENDIF(TEST_RUNNER)
