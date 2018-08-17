FIND_PACKAGE(MySQL REQUIRED)
if (NOT MYSQL_FOUND)
    MESSAGE(FATAL_ERROR "Could not find a usable libmysqlclient for the test suite!")
endif()
IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ".DLL")
    
    find_library(
        LIBMYSQL_RUNTIME_LIBRARY
        NAMES LIBMYSQL libmysql
        hintS $ENV{LIBMYSQL_RUNTIME_DIRECTORY}
    )
    if (${LIBMYSQL_RUNTIME_LIBRARY} STREQUAL "LIBMYSQL_RUNTIME_LIBRARY-NOTFOUND")
        MESSAGE(FATAL_ERROR "Could not find a usable MySQL runtime library libmysql.dll!\nYou can hint cmake by setting the environment variable LIBMYSQL_RUNTIME_DIRECTORY.")
    ENDIF()
    MESSAGE(STATUS "Found MySQL runtime library: ${LIBMYSQL_RUNTIME_LIBRARY}")
    
    SET(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES_BAK}")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES_BAK}")
ENDIF(WIN32)

include_directories(${MYSQL_INCLUDE_DIR})
find_package(Threads)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

ENABLE_TESTING()
macro(compile_test arg)
  add_executable(${arg} ${arg}.cpp)
  target_link_libraries(${arg} mcsapi ${LIBUV_LIBRARY} ${LIBXML2_LIBRARIES} ${GTEST_BOTH_LIBRARIES} ${MYSQL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
  add_test(${arg} ${arg})
endmacro(compile_test)

compile_test(version)
compile_test(commit)
compile_test(million-row)
compile_test(chained)
compile_test(dataconvert-int)
compile_test(dataconvert-double)
compile_test(dataconvert-string)
compile_test(dataconvert-datetime)
compile_test(dataconvert-decimal)
compile_test(dataconvert-null)
compile_test(bad-usage)
compile_test(truncation)
compile_test(summary)
compile_test(system-catalog)
compile_test(char4)
compile_test(mcol1160)
compile_test(mcol1408)

# On Windows copy the required shared libraries to execute the tests into the test folder as well.
IF(WIN32)
    ADD_CUSTOM_TARGET(copy_required_shared_test_libraries ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${LIBXML2_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND} -E copy ${LIBICONV_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND} -E copy ${LIBUV_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND} -E copy ${LIBMYSQL_RUNTIME_LIBRARY} "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mcsapi> "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
        DEPENDS mcsapi version
        COMMENT "Copy the shared libraries for executing the tests to the test directory"
    )
ENDIF(WIN32)
