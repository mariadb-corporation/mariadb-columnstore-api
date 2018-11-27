#!/bin/bash
#
# A bash script to build a windows DLL. Run from e.g. gitbash
# from within Windows (not Linux!)
#
# Does NOT build the java interface, but you can change that
# by editing this source code.
#



#Make sure we are working in our source directory. The
#script is in ./extra so go up one directory.
cd $(dirname $BASH_SOURCE)
cd ..

BUILD_DIR="build-win"
BUILD_CONFIG="RelWithDebInfo"
JAVA="OFF"

OUTPUT_DLL=`pwd`"/$BUILD_DIR/src/$BUILD_CONFIG/mcsapi.dll"


#---------------------------------
# Do not configure below this line
#---------------------------------

if [ -z "$PROGRAMFILES" ]; then
    echo "ERROR: This is a gitbash/windows script!"
    echo
    echo "    (PROGRAMFILES is not set, so this looks to be otherwise!)"
    exit 1
fi

if [ -z "$SUPPLEMENTAL_LIB_DIR" ]; then
    SUPPLEMENTAL_LIB_DIR=`pwd`"/mcsapi-windows-library-collection"
fi

if [ ! -d "$SUPPLEMENTAL_LIB_DIR" ]; then
    echo "ERROR: $SUPPLEMENTAL_LIB_DIR does not exist."
    echo "Please download the pre-compiled collection of"
    echo "dependant libraries as outlined on"
    echo "https://github.com/mariadb-corporation/mariadb-columnstore-api"
    echo "and set the environment variable SUPPLEMENTAL_LIB_DIR to the"
    echo "unzipped directory, or put the extracted files in"
    echo "$SUPPLEMENTAL_LIB_DIR"
    exit 1
fi

SWIG_DIR="/c/opt/swigwin-3.0.12"

if [ "$SWIG_DIR" = "" ]; then
    echo "ERROR: Please set the environment variable SWIG_DIR to"
    echo "  the directory containing the unzipped swig install."
    echo
    echo "e.g.: export SWIG_DIR=\"/c/opt/swigwing\""
    exit 1
fi

if [ "$PKG_CONFIG" = "" ]; then
    PKG_CONFIG="$(which pkg-config)"
fi

if [ "$PKG_CONFIG" = "" ]; then
    echo "ERROR: Please make sure pkg-config is either in the PATH or"
    echo "  set the environment variable PKG_CONFIG to point to the"
    echo "  executable."
    exit 1
fi

if [ -e "CMakeCache.txt" ]; then
    echo "ERROR: You have a CMakeCache.txt in your root directory!"
    echo "  This will result in a conflict and incorrect builds."
    echo "  Please clean up your previous cmake"
    exit 1
fi

if [ ! -d "$BUILD_DIR" ]; then
    if ! mkdir -p "$BUILD_DIR"; then echo "FAILED to mkdir $BUILD_DIR"; exit 1; fi
fi

if ! cd "$BUILD_DIR"; then echo "FAILED to cd $BUILD_DIR"; exit 1; fi

export LIBXML2_RUNTIME_DIRECTORY="$SUPPLEMENTAL_LIB_DIR/lib"
export LIBICONV_RUNTIME_DIRECTORY="$SUPPLEMENTAL_LIB_DIR/lib"
export LIBUV_RUNTIME_DIRECTORY="$SUPPLEMENTAL_LIB_DIR/lib"

if [ ! -d "$LIBXML2_RUNTIME_DIRECTORY" ]; then
    echo "ERROR: $LIBXML2_RUNTIME_DIRECTORY does not exist or is not a directory?"
    exit 1
fi

if [ ! -e "$SUPPLEMENTAL_LIB_DIR/lib/libxml2.lib" ]; then
    echo "ERROR: $SUPPLEMENTAL_LIB_DIR/lib/libxml2.lib does not exist?"
    echo
    echo "Please make sure that $SUPPLEMENTAL_LIB_DIR has both"
    echo "lib and include subdirectories from the dependent libraries"
    echo "zip."
    exit 1
fi

if ! cmake -G "Visual Studio 15 2017 Win64" .. \
    -DLIBXML2_INCLUDE_DIR:PATH="$SUPPLEMENTAL_LIB_DIR/include" \
    -DLIBXML2_LIBRARIES:PATH="$SUPPLEMENTAL_LIB_DIR/lib/libxml2.lib" \
    -DLIBUV_INCLUDE_DIR:PATH="$SUPPLEMENTAL_LIB_DIR/include" \
    -DLIBUV_LIBRARY:PATH="$SUPPLEMENTAL_LIB_DIR/lib/libuv.lib" \
    -DSNAPPY_INCLUDE_DIR:PATH="$SUPPLEMENTAL_LIB_DIR/include" \
    -DSNAPPY_LIBRARY:PATH="$SUPPLEMENTAL_LIB_DIR/lib/snappy.lib" \
    -DBOOST_INCLUDEDIR:PATH="$SUPPLEMENTAL_LIB_DIR/include" \
    -DSWIG_DIR:PATH="$SWIG_DIR" \
    -DSWIG_EXECUTABLE:PATH="$SWIG_DIR/swig.exe" \
    -DJAVA=$JAVA; then
    echo "FAILED to configure w/cmake"
    exit 1
fi


echo
echo
echo "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"
echo
echo


if ! cmake --build . --config "$BUILD_CONFIG"; then
    echo "FAILED to build w/cmake"
    exit 1
fi


echo
echo
echo "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"
echo
echo


echo -n "Build Complete at "
date

if [ -e "$OUTPUT_DLL" ]; then
    echo "Look For: $OUTPUT_DLL"
else
    echo "Hrm, DLL Not Produced? Looked for $OUTPUT_DLL"
fi

