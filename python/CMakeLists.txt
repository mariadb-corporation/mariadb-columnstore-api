IF ((NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/CMakeLists.txt"))
  FIND_PROGRAM(GIT_EXECUTABLE git)
  EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" submodule init
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")
  EXECUTE_PROCESS(COMMAND "${GIT_EXECUTABLE}" submodule update
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")
ENDIF()

IF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/CMakeLists.txt")
  MESSAGE(STATUS "Can't build pybind11 engine - missing CMakeLists.txt in pybind11 directory. Try \"git submodule init; git submodule update\".")
  RETURN()
ENDIF()

include_directories(pybind11/include ../libmcsapi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_MYMCSAPI")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(pybind11)
pybind11_add_module(pymcsapi pymcsapi.cpp)
target_link_libraries(pymcsapi mcsapi)
set_target_properties(pymcsapi PROPERTIES CXX_VISIBILITY_PRESET hidden
    VERSION ${MCSAPI_VERSION_STRING}
    SOVERSION ${MCSAPI_VERSION_MAJOR}
)

IF(RUN_CPPCHECK)
  add_cppcheck(mcsapi FORCE)
ENDIF(RUN_CPPCHECK)

INSTALL(TARGETS pymcsapi DESTINATION ${CMAKE_INSTALL_LIBDIR})
