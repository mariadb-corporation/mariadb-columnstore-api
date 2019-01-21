cmake_minimum_required(VERSION 2.8)

include_directories(../..)

set(SPARK_SCALA_MCSAPI_CONNECTOR_LIB
  "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/spark-scala-mcsapi-connector-${MCSAPI_VERSION_STRING}.jar"
)

IF(UNIX)
    ADD_CUSTOM_COMMAND(OUTPUT ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB} 
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} build -x test
        COMMAND ${CMAKE_COMMAND} -E create_symlink spark-scala-mcsapi-connector-${MCSAPI_VERSION_STRING}.jar "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/spark-scala-mcsapi-connector.jar"
        DEPENDS javamcsapi
        COMMENT "building the spark-scala-mcsapi-connector library"
    )
ENDIF(UNIX)
IF(WIN32)
    ADD_CUSTOM_COMMAND(OUTPUT ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB} 
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew.bat" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pversion=${MCSAPI_VERSION_STRING} build -x test
        DEPENDS javamcsapi
        COMMENT "building the spark-scala-mcsapi-connector library"
    )
ENDIF(WIN32)

ADD_CUSTOM_TARGET(spark-scala-mcsapi-connector ALL DEPENDS ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB})

IF(TEST_RUNNER)
  IF(UNIX)
    add_test(NAME SparkScalaConnector_BasicTest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pjava.library.path=${CMAKE_CURRENT_BINARY_DIR}/../../java -Pversion=${MCSAPI_VERSION_STRING} test)
  ENDIF(UNIX)
  IF(WIN32)
    add_test(NAME SparkScalaConnector_BasicTest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew.bat" -p ${CMAKE_CURRENT_SOURCE_DIR} -Pjava.library.path=${CMAKE_CURRENT_BINARY_DIR}/../../java/$<CONFIGURATION> -Pversion=${MCSAPI_VERSION_STRING} test)
  ENDIF(WIN32)
ENDIF(TEST_RUNNER)

IF(UNIX)
    INSTALL(FILES ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT spark)
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/spark-scala-mcsapi-connector.jar" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT spark)
ENDIF(UNIX)
IF(WIN32)
    INSTALL(FILES ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB} DESTINATION "${CMAKE_INSTALL_LIBDIR}/java" COMPONENT spark)
ENDIF(WIN32)
