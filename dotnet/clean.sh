#!/bin/bash
#
# Clean the dotnet directory from build and cmake files.

#Make sure we are working in our source directory.
cd $(dirname $BASH_SOURCE)

rm -rfv build-linux build-win64 *.vcxproj* \
    CMakeCache.txt CMakeFiles cmake_install.cmake \
    dotnet_mcsapi_wrap.cxx Project.sln Makefile \
    Debug Win32 \
    lib-win64/dotnet_mcsapi.* \
    lib-linux/dotnet*.so \
    src/obj src/bin src/*.cs \
    test/obj test/bin test/*.log \
    example/obj example/bin
