set(BULK_INSERT_FILES
    bulk_insert.cpp
)

add_executable(bulk_insert ${BULK_INSERT_FILES})

target_link_libraries(bulk_insert mcsapi ${LIBUV_LIBRARY} ${LIBXML2_LIBRARIES})

INSTALL(TARGETS bulk_insert
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

IF(RUN_CPPCHECK)
  add_cppcheck(bulk_insert FORCE)
ENDIF(RUN_CPPCHECK)
