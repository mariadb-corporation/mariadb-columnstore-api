cmake_minimum_required(VERSION 2.8.11)

include_directories(..)

#Compiling the shared library
set(SWIG_PYTHON2_WRAPPER_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi_wrap.cxx")
add_custom_command(OUTPUT ${SWIG_PYTHON2_WRAPPER_SOURCE}
    COMMAND ${SWIG_EXECUTABLE} -c++ -python -I${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi.i
    COMMENT "Generating the Python wrapper source code")

add_library(_pymcsapi SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi_wrap.cxx")
set_target_properties(_pymcsapi PROPERTIES OUTPUT_NAME "_pymcsapi")
set_target_properties(_pymcsapi PROPERTIES PREFIX "")
set_target_properties(_pymcsapi PROPERTIES VERSION ${MCSAPI_VERSION_STRING}
    SOVERSION ${MCSAPI_VERSION_MAJOR}
)
IF(WIN32)
  set_target_properties(_pymcsapi PROPERTIES SUFFIX ".pyd")
ENDIF(WIN32)
target_include_directories(_pymcsapi PRIVATE ${PYTHON2_INCLUDE_DIR})
IF(UNIX)
  target_link_libraries(_pymcsapi mcsapi)
ENDIF(UNIX)
IF(WIN32)
  target_link_libraries(_pymcsapi mcsapi ${PYTHON2_LIBRARY})
ENDIF(WIN32)

#Tests
IF(TEST_RUNNER)
  add_test(NAME test_basic_python COMMAND ${PYTHON2_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_basic.py)
  add_test(NAME test_million_row_python COMMAND ${PYTHON2_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_million_row.py)
  add_test(NAME test_exception_python COMMAND ${PYTHON2_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_exception.py)
  add_test(NAME test_status_python COMMAND ${PYTHON2_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_status.py)
  add_test(NAME test_table_lock_python COMMAND ${PYTHON2_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_tableLocks.py)
ENDIF(TEST_RUNNER)

#Install
IF(UNIX)
  execute_process(COMMAND ${PYTHON2_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE PYTHON2_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(PYTHON2_SITE_PACKAGES ${PYTHON2_SITE_PACKAGES} PARENT_SCOPE)
  install(TARGETS _pymcsapi DESTINATION ${PYTHON2_SITE_PACKAGES} COMPONENT python)
  install(FILES pymcsapi.py DESTINATION ${PYTHON2_SITE_PACKAGES} COMPONENT python)
ENDIF(UNIX)
IF(WIN32)
  execute_process(COMMAND ${PYTHON2_EXECUTABLE} -c "import sys; print('%d.%d' % (sys.version_info[0], sys.version_info[1]))" OUTPUT_VARIABLE PYTHON2_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(TARGETS _pymcsapi DESTINATION "${CMAKE_INSTALL_LIBDIR}/python${PYTHON2_VERSION}" COMPONENT python)
  install(FILES pymcsapi.py DESTINATION "${CMAKE_INSTALL_LIBDIR}/python${PYTHON2_VERSION}" COMPONENT python)
  # set global variables to be substituted by WIX package manager
  set(PYTHON2_PYMCSAPI_PY "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi.py" PARENT_SCOPE)
  set(PYTHON2_VERSION ${PYTHON2_VERSION} PARENT_SCOPE)
ENDIF(WIN32)

