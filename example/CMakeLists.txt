set(BASIC_BULK_INSERT_FILES
    basic_bulk_insert.cpp
)

add_executable(basic_bulk_insert ${BASIC_BULK_INSERT_FILES})

target_link_libraries(basic_bulk_insert mcsapi ${LIBUV_LIBRARY} ${LIBXML2_LIBRARIES})

INSTALL(TARGETS basic_bulk_insert
    RUNTIME DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
    COMPONENT cpp-devel
)

INSTALL(FILES "basic_bulk_insert.cpp"
    "basic_bulk_insert.sql"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
    COMPONENT cpp-devel
)


set(ADVANCED_BULK_INSERT_FILES
    advanced_bulk_insert.cpp
)

add_executable(advanced_bulk_insert ${ADVANCED_BULK_INSERT_FILES})

target_link_libraries(advanced_bulk_insert mcsapi ${LIBUV_LIBRARY} ${LIBXML2_LIBRARIES})

INSTALL(TARGETS advanced_bulk_insert
    RUNTIME DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
    COMPONENT cpp-devel
)
INSTALL(FILES "advanced_bulk_insert.cpp"
    "advanced_bulk_insert.sql"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/example"
    COMPONENT cpp-devel
)

# On Windows install in addition the dependent shared libraries (libuv, libxml and libiconv) used during building to get the examples executable
IF(WIN32)
    INSTALL(FILES ${LIBXML2_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_DOCDIR}/example" COMPONENT cpp-devel)
    INSTALL(FILES ${LIBICONV_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_DOCDIR}/example" COMPONENT cpp-devel)
    INSTALL(FILES ${LIBUV_RUNTIME_LIBRARY} DESTINATION "${CMAKE_INSTALL_DOCDIR}/example" COMPONENT cpp-devel)
ENDIF(WIN32)

IF(RUN_CPPCHECK)
  add_cppcheck(basic_bulk_insert FORCE)
  add_cppcheck(advanced_bulk_insert FORCE)
ENDIF(RUN_CPPCHECK)
