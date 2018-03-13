cmake_minimum_required(VERSION 2.8)

include_directories(../..)

set(SPARK_SCALA_MCSAPI_CONNECTOR_LIB
  "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/spark-scala-mcsapi-connector.jar"
)

ADD_CUSTOM_COMMAND(OUTPUT ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB} 
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -b ${CMAKE_CURRENT_SOURCE_DIR}/build.gradle -c ${CMAKE_CURRENT_SOURCE_DIR}/settings.gradle  build -x test
	DEPENDS javamcsapi
	COMMENT "building the spark-scala-mcsapi-connector library"
)

ADD_CUSTOM_TARGET(spark-scala-mcsapi-connector ALL DEPENDS ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB})

IF(TEST_RUNNER)
  add_test(NAME SparkScalaConnector_BasicTest COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/gradlew" -b ${CMAKE_CURRENT_SOURCE_DIR}/build.gradle -c ${CMAKE_CURRENT_SOURCE_DIR}/settings.gradle  test)
ENDIF(TEST_RUNNER)

INSTALL(FILES ${SPARK_SCALA_MCSAPI_CONNECTOR_LIB} DESTINATION ${CMAKE_INSTALL_LIBDIR})
