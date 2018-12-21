# Library versioning
# Increment if interfaces have been removed or changed
set(MCSAPI_VERSION_MAJOR 1)
# Incement if source code has changed, zero if major is incremented
set(MCSAPI_VERSION_MINOR 2)
# Increment if interfaces have beed added, set to zero if minor is changed
set(MCSAPI_VERSION_PATCH 3)
set(MCSAPI_VERSION_STRING ${MCSAPI_VERSION_MAJOR}.${MCSAPI_VERSION_MINOR}.${MCSAPI_VERSION_PATCH})

# Version number to echo
exec_program("git"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "describe --match=NeVeRmAtCh --always --dirty"
    OUTPUT_VARIABLE GIT_VERSION)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 3)
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${GIT_VERSION}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gitversionAPI.in
                ${CMAKE_CURRENT_BINARY_DIR}/gitversionAPI)
