SET(HEADERS
    mcsapi.h
    mcsapi_bulk.h
    mcsapi_exception.h
    mcsapi_driver.h
    mcsapi_types.h
    visibility.h
)

INSTALL(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libmcsapi)
