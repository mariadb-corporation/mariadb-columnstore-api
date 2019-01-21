#!/bin/bash
#
# bash / (git)bash script to build a nuget package.
#
# Uses modern bash features such as arrays.
#
# Also this script plays well with build-win-x64.sh
#  (https://gist.github.com/TikiBill/3dc83d90e5337eb168da82c207f077e3)


#Make sure we are working in our source directory.
cd $(dirname $BASH_SOURCE)

WARNINGS=()

if [ -z "$SWIG_EXECUTABLE" ]; then
    SWIG_EXECUTABLE=$(which swig 2>/dev/null)
fi

if [ -z "$PKG_CONFIG" ]; then
    PKG_CONFIG=$(which pkg-config 2>/dev/null)
fi

let MISSING=0
MISSING_DLLS=""    

if [ -z "$PROGRAMFILES" ]; then
    TARGET="linux-x64"
    BUILD_DIR="build-$TARGET"
    OUTPUT_DLL="glue-lib/dotnet_mcsapi.so"
    DEST_LIB_DIR="lib-$TARGET"

    # Assuming the README was followed.
    API_DLL_DIR="../build-linux-x64/src"    
    REQUIRED_DLLS="libmcsapi.so libmcsapi.a"

    if [ ! -d "$DEST_LIB_DIR" ]; then
        if ! mkdir "$DEST_LIB_DIR"; then echo "FAILED to mkdir $DEST_LIB_DIR"; exit 1; fi
    fi

else
    # Windows Config
    TARGET="win-x64"
    BUILD_DIR="build-$TARGET"
    BUILD_CONFIG="RelWithDebInfo"
    OUTPUT_DLL="glue-lib/$BUILD_CONFIG/dotnet_mcsapi.dll"
    DEST_LIB_DIR="lib-$TARGET"

    #Assuming the build-win-x64 script was used:
    API_DLL_DIR="../build-win-x64/src/RelWithDebInfo"

    REQUIRED_DLLS="mcsapi.dll mcsapi.lib libiconv.dll libuv.dll libxml2.dll"
fi

if [ ! -d "$DEST_LIB_DIR" ]; then
    if ! mkdir "$DEST_LIB_DIR"; then echo "FAILED to mkdir $DEST_LIB_DIR"; exit 1; fi
fi

for DLL in $REQUIRED_DLLS; do
    if [ ! -e "./$DEST_LIB_DIR/$DLL" ]; then
        if [ ! -z "$MCSAPI_BUILD_DEPENDENCY_DIR" -a -e "$MCSAPI_BUILD_DEPENDENCY_DIR/lib/$DLL" ]; then
            echo "NOTE: Missing ./$DEST_LIB_DIR/$DLL but found it in $MCSAPI_BUILD_DEPENDENCY_DIR/lib"
            if cp -v "$MCSAPI_BUILD_DEPENDENCY_DIR/lib/$DLL" "./$DEST_LIB_DIR/$DLL"; then
                WARNINGS+=("Copied $MCSAPI_BUILD_DEPENDENCY_DIR/lib/$DLL  -->  ./$DEST_LIB_DIR/$DLL")
            else
                echo "ERROR Copy failed!"
                exit 1
            fi
            echo
        elif [ -e "$API_DLL_DIR/$DLL" ]; then
            echo "NOTE: Missing ./$DEST_LIB_DIR/$DLL but found it in $API_DLL_DIR"
            if cp -v "$API_DLL_DIR/$DLL" "./$DEST_LIB_DIR/$DLL"; then
                WARNINGS+=("Copied $API_DLL_DIR/$DLL  -->  ./$DEST_LIB_DIR/$DLL")
            else
                echo "ERROR Copy failed!"
                exit 1
            fi
            echo
        else
            MISSING=$((MISSING+1))
            echo "ERROR: Missing ./$DEST_LIB_DIR/$DLL"
            MISSING_DLLS="$MISSING_DLLS ./$DEST_LIB_DIR/$DLL"
        fi
    elif [ -e "$API_DLL_DIR/$DLL" -a "$API_DLL_DIR/$DLL" -nt "./$DEST_LIB_DIR/$DLL" ]; then
        WARNINGS+=("WARNING: $API_DLL_DIR/$DLL is newer than ./$DEST_LIB_DIR/$DLL")
        echo "${WARNINGS[-1]}"
    fi
done

if [ $MISSING -gt 0 ]; then
    echo
    echo "ERROR: Missing some required DLLs:$MISSING_DLLS"
    echo
    echo "  The DLLs are needed for packing a complete NuGet package (and testing)."
    exit 1
fi

#Compare versions in a crude way.
VERSION_MAJOR=$(cat ../cmake/version.cmake |grep -E 'set\( *VERSION_MAJOR *[0-9]+ *\)' | sed -r 's/.*(VERSION_MAJOR +([0-9]+)).*/\2/')
VERSION_MINOR=$(cat ../cmake/version.cmake |grep -E 'set\( *VERSION_MINOR *[0-9]+ *\)' | sed -r 's/.*(VERSION_MINOR +([0-9]+)).*/\2/')
VERSION_PATCH=$(cat ../cmake/version.cmake |grep -E 'set\( *VERSION_PATCH *[0-9]+ *\)' | sed -r 's/.*(VERSION_PATCH +([0-9]+)).*/\2/')
API_VERSION="$VERSION_MAJOR.$VERSION_MINOR.$VERSION_PATCH"
NUGET_VERSION=$(cat ./src/ColumnStoreDotNet.csproj | grep -E '<PackageVersion>' | sed -r 's/.*<PackageVersion> *([0-9]+\.[0-9]+\.[0-9]+).*/\1/')

if [ "$API_VERSION" != "$NUGET_VERSION" ]; then
    WARNINGS+=("WARNING: API Version ($API_VERSION) Does Not Match NuGet Version ($NUGET_VERSION) -- Perhaps edit ColumnStoreDotNet.csproj?")
    echo "${WARNINGS[-1]}"
fi

OUTPUT_DLL_ABS=`pwd`"/$OUTPUT_DLL"

if [ -z "$SWIG_EXECUTABLE" ]; then
    echo "ERROR: swig executable not found/known."
    echo "  Please add swig to your path or set"
    echo "  the SWIG_EXECUTABLE environment variable."
    echo "  to point to the executable."
    exit 1
fi

if [ -z "$PKG_CONFIG" ]; then
    echo "ERROR: pkg-config executable not found/known."
    echo "  Please add pkg-config to your path or set"
    echo "  the PKG_CONFIG environment variable to point"
    echo "  to the executable."
    exit 1
fi


if [ -e "CMakeCache.txt" ]; then
    echo "ERROR: You have a CMakeCache.txt in the main dotnet directory!"
    echo "  This will result in a conflict and incorrect builds."
    echo "  Please clean up your previous cmake."
    exit 1
fi

if [ ! -d "$BUILD_DIR" ]; then
    if ! mkdir -p "$BUILD_DIR"; then echo "FAILED to mkdir $BUILD_DIR"; exit 1; fi
fi

if ! cd "$BUILD_DIR"; then echo "FAILED to cd $BUILD_DIR"; exit 1; fi

if [ "$TARGET" = "win-x64" ]; then
    SETUP_CMD="cmake -G \"Visual Studio 15 2017 Win64\" .. -DSWIG_EXECUTABLE:PATH=\"$SWIG_EXECUTABLE\""
    BUILD_CMD="cmake --build . --config \"$BUILD_CONFIG\""
elif [ "$TARGET" = "linux-x64" ]; then
    SETUP_CMD='cmake ..'
    BUILD_CMD="make"
else
    echo "ERROR: Unknown target '$TARGET'"
    exit 1
fi

echo
echo
echo "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"
echo "    $TARGET    PWD: " $(pwd)
echo "    RUNNING: $SETUP_CMD"
echo

if ! eval "$SETUP_CMD"; then
    echo "ERROR: Command Failed!"
    echo "Tried to run: $SETUP_CMD"
    exit 1
fi


echo
echo
echo "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"
echo "    $TARGET    PWD: " $(pwd)
echo "    RUNNING: $BUILD_CMD"
echo

if ! eval "$BUILD_CMD"; then
    echo "ERROR: Command Failed!"
    echo "Tried to run: $BUILD_CMD"
    exit 1
fi

if [ -e "$OUTPUT_DLL" ]; then
    CP_CMD="cp -v \"$OUTPUT_DLL\" \"../$DEST_LIB_DIR/\""
    echo -n "COPY: $CP_CMD    --->    "    
    if ! eval $CP_CMD; then "FAILED to $CP_CMD"; exit 1; fi
else
    echo "Hrm, DLL Not Produced? Looked for $OUTPUT_DLL"
    CP_CMD="(No Copy Done)"
fi


echo
echo
echo "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"
echo "        MariaDB ColumnStore API Version: $API_VERSION"
echo "  NuGet Package Version (without build): $NUGET_VERSION"
echo
echo "Sucssfully setup/built with commands:"
echo "  $SETUP_CMD"
echo "  $BUILD_CMD"
echo "  $CP_CMD"
echo

if [ ${#WARNINGS[@]} -gt 0 ]; then
    echo "*** NOTICE ***"
    for WARN in "${WARNINGS[@]}"; do
        echo "$WARN"
    done
    echo
fi
