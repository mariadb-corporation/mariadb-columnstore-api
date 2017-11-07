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
set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/src/main/java/com/mariadb/columnstore/api")
set_source_files_properties( javamcsapi.i PROPERTIES CPLUSPLUS ON)
swig_add_module(javamcsapi java javamcsapi.i)
swig_link_libraries(javamcsapi mcsapi)

# SWIG behaves differently in Ubuntu's CMake
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.0)
  ADD_CUSTOM_COMMAND(TARGET javamcsapi POST_BUILD
      COMMAND "${CMAKE_CURRENT_BINARY_DIR}/gradlew" jar
  )
  INSTALL(TARGETS javamcsapi DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
ELSE ()
  ADD_CUSTOM_COMMAND(TARGET _javamcsapi POST_BUILD
      COMMAND "${CMAKE_CURRENT_BINARY_DIR}/gradlew" jar
  )
  INSTALL(TARGETS _javamcsapi DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
ENDIF()
IF(TEST_RUNNER)
  add_test(NAME Java_BasicTest COMMAND "${CMAKE_CURRENT_BINARY_DIR}/gradlew" test)
ENDIF(TEST_RUNNER)

INSTALL(FILES build/libs/javamcsapi.jar DESTINATION  ${CMAKE_INSTALL_LIBDIR})
