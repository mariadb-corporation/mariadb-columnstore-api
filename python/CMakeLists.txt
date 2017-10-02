cmake_minimum_required(VERSION 2.8)

include_directories(..)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_MYMCSAPI")

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")
set_source_files_properties( pymcsapi.i PROPERTIES CPLUSPLUS ON)
swig_add_module(pymcsapi python pymcsapi.i)
swig_link_libraries(pymcsapi mcsapi ${PYTHON_LIBRARIES})

IF(TEST_RUNNER)
  find_package(PythonInterp REQUIRED)
  add_test(NAME test_basic COMMAND ${PYTHON_EXECUTABLE} -m pytest test/test_basic.py)
  add_test(NAME test_million_row COMMAND ${PYTHON_EXECUTABLE} -m pytest test/test_million_row.py)
ENDIF(TEST_RUNNER)

execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
install(TARGETS _pymcsapi DESTINATION ${PYTHON_SITE_PACKAGES})
install(FILES pymcsapi.py pymcsapi.pyc DESTINATION ${PYTHON_SITE_PACKAGES})


