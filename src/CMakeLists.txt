if (Boost_VERSION VERSION_LESS 105800)
    include_directories(${LIBXML2_INCLUDE_DIR} ../boost/multiprecision/include ../boost/type_traits/include ../boost/config/include ../boost/lexical_cast/include)
else()
    include_directories(${LIBXML2_INCLUDE_DIR})
endif()

IF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
ENDIF(UNIX)
IF(WIN32)
    #TODO find a way to define the __FILENAME__ in Windows so that the debug output doesn't contain the absolute path.
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__=\"$(notdir $<)\"")
ENDIF(WIN32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_MCSAPI")

if (HAVE_GET_TIME)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_GET_TIME=1")
endif(HAVE_GET_TIME)

set(SOURCE_FILES
    mcsapi_driver.cpp
    mcsapi_types.cpp
    mcsapi_bulk.cpp
    util_debug.cpp
    util_dataconvert.cpp
    util_messaging.cpp
    util_network.cpp
    util_commands.cpp
    util_structures.cpp
)

if(WIN32)
    set(SOURCE_FILES
        ${SOURCE_FILES}
        WIN32/time.cpp
    )
endif(WIN32)

add_library(mcsapi SHARED ${SOURCE_FILES})
add_library(mcsapi_static STATIC ${SOURCE_FILES})
set_target_properties(mcsapi PROPERTIES CXX_VISIBILITY_PRESET hidden
    VERSION ${MCSAPI_VERSION_STRING}
    SOVERSION ${MCSAPI_VERSION_MAJOR}
)
IF(UNIX)
    set_target_properties(mcsapi_static PROPERTIES OUTPUT_NAME mcsapi)
ENDIF(UNIX)
target_link_libraries(mcsapi ${SNAPPY_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBUV_LIBRARIES})
target_link_libraries(mcsapi_static ${SNAPPY_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBUV_LIBRARIES})

IF(RUN_CPPCHECK)
  add_cppcheck(mcsapi FORCE)
ENDIF(RUN_CPPCHECK)


INSTALL(TARGETS mcsapi DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
INSTALL(TARGETS mcsapi_static DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)

# On Windows install in addition the dependent shared libraries (libuv, libxml and libiconv) used during building
IF(WIN32)
    INSTALL(FILES ${LIBXML2_RUNTIME_LIBRARY} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
    INSTALL(FILES ${LIBICONV_RUNTIME_LIBRARY} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
    INSTALL(FILES ${LIBUV_RUNTIME_LIBRARY} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libs)
    # also place mcsapi.dll into the examples directory to get basic_bulk_insert.exe and advanced_bulk_insert.exe executable
    INSTALL(TARGETS mcsapi RUNTIME DESTINATION "${CMAKE_INSTALL_DOCDIR}/example" COMPONENT devel)
ENDIF(WIN32)