cmake_minimum_required(VERSION 2.8)

include_directories(../..)

IF(PYTHON2_AVAILABLE)
    IF(TEST_RUNNER)
        add_test(NAME pythonColumnStoreExporter COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_column_store_exporter.sh ${PYTHON2_EXECUTABLE} ipython)
    ENDIF(TEST_RUNNER)
    install(FILES columnStoreExporter.py DESTINATION ${PYTHON2_SITE_PACKAGES})
    install(FILES mcsapi_reserved_words.txt DESTINATION ${PYTHON2_SITE_PACKAGES})
ENDIF(PYTHON2_AVAILABLE)

IF(PYTHON3_AVAILABLE)
    IF(TEST_RUNNER)
        add_test(NAME python3ColumnStoreExporter COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_column_store_exporter.sh ${PYTHON3_EXECUTABLE} ipython3)
    ENDIF(TEST_RUNNER)
    install(FILES columnStoreExporter.py DESTINATION ${PYTHON3_SITE_PACKAGES})
    install(FILES mcsapi_reserved_words.txt DESTINATION ${PYTHON3_SITE_PACKAGES})
ENDIF(PYTHON3_AVAILABLE)
