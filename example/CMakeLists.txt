set(BASIC_BULK_INSERT_FILES
    basic_bulk_insert.cpp
)

add_executable(basic_bulk_insert ${BASIC_BULK_INSERT_FILES})

target_link_libraries(basic_bulk_insert mcsapi ${LIBUV_LIBRARY} ${LIBXML2_LIBRARIES})

INSTALL(TARGETS basic_bulk_insert
    RUNTIME DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
)

INSTALL(FILES "basic_bulk_insert.cpp"
    "basic_bulk_insert.sql"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
)


set(ADVANCED_BULK_INSERT_FILES
    advanced_bulk_insert.cpp
)

add_executable(advanced_bulk_insert ${ADVANCED_BULK_INSERT_FILES})

target_link_libraries(advanced_bulk_insert mcsapi ${LIBUV_LIBRARY} ${LIBXML2_LIBRARIES})

INSTALL(TARGETS advanced_bulk_insert
    RUNTIME DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
)
INSTALL(FILES "advanced_bulk_insert.cpp"
    "advanced_bulk_insert.sql"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
)


IF(RUN_CPPCHECK)
  add_cppcheck(basic_bulk_insert FORCE)
  add_cppcheck(advanced_bulk_insert FORCE)
ENDIF(RUN_CPPCHECK)
