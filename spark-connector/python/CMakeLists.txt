cmake_minimum_required(VERSION 2.8)

include_directories(../..)

IF(PYTHON2_AVAILABLE)
    IF(TEST_RUNNER)
        add_test(NAME spark-python-connector COMMAND ./test/test_spark_python_mcsapi_connector.sh ${PYTHON2_EXECUTABLE} ipython)
    ENDIF(TEST_RUNNER)
    install(FILES sparkPythonMcsapiConnector.py DESTINATION ${PYTHON2_SITE_PACKAGES})
ENDIF(PYTHON2_AVAILABLE)

IF(PYTHON3_AVAILABLE)
    IF(TEST_RUNNER)
        add_test(NAME spark-python3-connector COMMAND ./test/test_spark_python_mcsapi_connector.sh ${PYTHON3_EXECUTABLE} ipython3)
    ENDIF(TEST_RUNNER)
    install(FILES sparkPythonMcsapiConnector.py DESTINATION ${PYTHON3_SITE_PACKAGES})
ENDIF(PYTHON3_AVAILABLE)
