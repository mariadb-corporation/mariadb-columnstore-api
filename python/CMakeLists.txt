cmake_minimum_required(VERSION 2.8)

#MESSAGE(STATUS "${CMAKE_CXX_FLAGS}")

execute_process(COMMAND python -c "print(1)" OUTPUT_VARIABLE PYTHON2_AVAILABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND python3 -c "print(1)" OUTPUT_VARIABLE PYTHON3_AVAILABLE OUTPUT_STRIP_TRAILING_WHITESPACE)

IF(PYTHON2_AVAILABLE OR PYTHON3_AVAILABLE)
  set(SWIG_PYTHON_WRAPPER
      "pymcsapi.py"
      "pymcsapi_wrap.cxx"
  )
  add_custom_command(OUTPUT ${SWIG_PYTHON_WRAPPER}
                     COMMAND swig -c++ -python -I.. pymcsapi.i
                     COMMENT "Building the Python wrapper")
  IF(PYTHON2_AVAILABLE)
    set(PYTHON2_LIBRARY_OBJECT "_pymcsapi.so")
    add_custom_command(OUTPUT ${PYTHON2_LIBRARY_OBJECT}
                       COMMAND python setup.py build_ext --inplace
                       DEPENDS ${SWIG_PYTHON_WRAPPER}
                       COMMENT "Building the Python2 library")
    add_custom_target(_pymcsapi ALL DEPENDS ${PYTHON2_LIBRARY_OBJECT})
    IF(TEST_RUNNER)
      add_test(NAME test_basic COMMAND python -m pytest test/test_basic.py)
      add_test(NAME test_million_row COMMAND python -m pytest test/test_million_row.py)
    ENDIF(TEST_RUNNER)
    execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE PYTHON2_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
    install(FILES _pymcsapi.so DESTINATION ${PYTHON2_SITE_PACKAGES})
    install(FILES pymcsapi.py DESTINATION ${PYTHON2_SITE_PACKAGES})
  ENDIF(PYTHON2_AVAILABLE)
  IF(PYTHON3_AVAILABLE)
    set(PYTHON3_LIBRARY_OBJECT "_pymcsapi3.so")
    add_custom_command(OUTPUT ${PYTHON3_LIBRARY_OBJECT}
                       COMMAND python3 setup.py build_ext --inplace
                       DEPENDS ${SWIG_PYTHON_WRAPPER}
                       COMMENT "Building the Python3 library")
    add_custom_target(_pymcsapi3 ALL DEPENDS ${PYTHON3_LIBRARY_OBJECT})
    IF(TEST_RUNNER)
      add_test(NAME test_basic COMMAND python3 -m pytest test/test_basic.py)
      add_test(NAME test_million_row COMMAND python3 -m pytest test/test_million_row.py)
    ENDIF(TEST_RUNNER)
    execute_process(COMMAND python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())" OUTPUT_VARIABLE PYTHON3_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
    install(FILES _pymcsapi3.so DESTINATION ${PYTHON3_SITE_PACKAGES})
    install(FILES pymcsapi.py DESTINATION ${PYTHON3_SITE_PACKAGES})
  ENDIF(PYTHON3_AVAILABLE)
ENDIF(PYTHON2_AVAILABLE OR PYTHON3_AVAILABLE)
