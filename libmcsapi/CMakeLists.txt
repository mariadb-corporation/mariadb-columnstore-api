SET(HEADERS
    mcsapi.h
    mcsapi_bulk.h
    mcsapi_exception.h
    mcsapi_driver.h
    mcsapi_types.h
    visibility.h
)

IF(WIN32)
  INSTALL(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libmcsapi COMPONENT cpp_devel)
ELSE(WIN32)
  INSTALL(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libmcsapi COMPONENT cpp-devel)
ENDIF(WIN32)
