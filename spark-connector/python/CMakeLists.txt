cmake_minimum_required(VERSION 2.8)

include_directories(../..)

IF(PYTHON2_AVAILABLE)
    IF(TEST_RUNNER)
        add_test(NAME spark-python-connector COMMAND ${PYTHON2_EXECUTABLE} -m pytest sparkPythonMcsapiConnectorTest.py)
    ENDIF(TEST_RUNNER)
    install(FILES sparkPythonMcsapiConnector.py DESTINATION ${PYTHON2_SITE_PACKAGES})
ENDIF(PYTHON2_AVAILABLE)

IF(PYTHON3_AVAILABLE)
    IF(TEST_RUNNER)
        add_test(NAME spark-python-connector3 COMMAND ${PYTHON3_EXECUTABLE} -m pytest sparkPythonMcsapiConnectorTest.py)
    ENDIF(TEST_RUNNER)
    install(FILES sparkPythonMcsapiConnector.py DESTINATION ${PYTHON3_SITE_PACKAGES})
ENDIF(PYTHON3_AVAILABLE)
