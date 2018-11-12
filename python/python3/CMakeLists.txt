cmake_minimum_required(VERSION 2.8.11)

include_directories(..)

set(SWIG_PYTHON3_WRAPPER_SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi_wrap.cxx")
add_custom_command(OUTPUT ${SWIG_PYTHON3_WRAPPER_SOURCE}
  COMMAND ${SWIG_EXECUTABLE} -c++ -python -py3 -I${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi.i
  COMMENT "Generating the Python 3 wrapper source code")

add_library(_pymcsapi3 SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi_wrap.cxx")
set_target_properties(_pymcsapi3 PROPERTIES OUTPUT_NAME "_pymcsapi")
set_target_properties(_pymcsapi3 PROPERTIES PREFIX "")
set_target_properties(_pymcsapi3 PROPERTIES VERSION ${MCSAPI_VERSION_STRING}
    SOVERSION ${MCSAPI_VERSION_MAJOR}
)
IF(WIN32)
  set_target_properties(_pymcsapi3 PROPERTIES SUFFIX ".pyd")
ENDIF(WIN32)
  target_include_directories(_pymcsapi3 PRIVATE ${PYTHON3_INCLUDE_DIR})
IF(UNIX)
  target_link_libraries(_pymcsapi3 mcsapi)
ENDIF(UNIX)
IF(WIN32)
  target_link_libraries(_pymcsapi3 mcsapi ${PYTHON3_LIBRARY})
ENDIF(WIN32)
    
#Tests
IF(TEST_RUNNER)
  add_test(NAME test_basic_python3 COMMAND ${PYTHON3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_basic.py)
  add_test(NAME test_million_row_python3 COMMAND ${PYTHON3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_million_row.py)
  add_test(NAME test_exception_python3 COMMAND ${PYTHON3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_exception.py)
  add_test(NAME test_status_python3 COMMAND ${PYTHON3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_status.py)
ENDIF(TEST_RUNNER)
    
#Install
IF(UNIX)
  execute_process(COMMAND ${PYTHON3_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE PYTHON3_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(PYTHON3_SITE_PACKAGES ${PYTHON3_SITE_PACKAGES} PARENT_SCOPE)
  install(TARGETS _pymcsapi3 DESTINATION ${PYTHON3_SITE_PACKAGES} COMPONENT python3)
  install(FILES pymcsapi.py DESTINATION ${PYTHON3_SITE_PACKAGES} COMPONENT python3)
ENDIF(UNIX)
IF(WIN32)
  execute_process(COMMAND ${PYTHON3_EXECUTABLE} -c "import sys; print('%d.%d' % (sys.version_info[0], sys.version_info[1]))" OUTPUT_VARIABLE PYTHON3_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  install(TARGETS _pymcsapi3 DESTINATION "${CMAKE_INSTALL_LIBDIR}/python${PYTHON3_VERSION}" COMPONENT python3)
  install(FILES pymcsapi.py DESTINATION "${CMAKE_INSTALL_LIBDIR}/python${PYTHON3_VERSION}" COMPONENT python3)
  # set global variables to be substituted by WIX package manager
  set(PYTHON3_PYMCSAPI_PY "${CMAKE_CURRENT_SOURCE_DIR}/pymcsapi.py" PARENT_SCOPE)
  set(PYTHON3_VERSION ${PYTHON3_VERSION} PARENT_SCOPE)
ENDIF(WIN32)

