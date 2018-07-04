#!/bin/bash

if [ $# -ge 3 ]; then
    MAJOR_VERSION=$1
    MINOR_VERSION=$2
    PATCH_LEVEL=$3
else
    echo $0 MAJOR_VERSION MINOR_VERSION PATCH_LEVEL
    exit -1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )" #get the absolute diretory of this script
cd $DIR

# build all tagged versions mcsapi and javamcsapi with the same MAJOR and MINOR version
for tag in $( git tag -l columnstore-$MAJOR_VERSION.$MINOR_VERSION.* ); do
    #filter versions prior 1.1.3 as these are unstable and use _javamcsapi.so instead of javamcsapi.so
    IFS='.' read -r -a CHECK_VERSION <<< ${tag:12}
    if [ ${CHECK_VERSION[0]} -ge 2 -o ${CHECK_VERSION[1]} -ge 2 -o ${CHECK_VERSION[2]} -ge 3 ]; then
        echo "building mcsapi and javamcsapi for tag: $tag"
        git clone https://github.com/mariadb-corporation/mariadb-columnstore-api.git
        mv mariadb-columnstore-api $tag
        cd $tag
        git checkout tags/$tag
        cmake -DTEST_RUNNER=ON -DSPARK_CONNECTOR=OFF -DPYTHON=OFF -DJAVA=ON -DRUN_CPPCHECK=OFF -DBUILD_DOCS=OFF .
        make -j2
        cd $DIR

        # prepare the backward compatibility test
        cp -r $tag{,.backward}
        rm -f $DIR/$tag.backward/java/build/libs/javamcsapi*.jar
        cp $DIR/../build/libs/javamcsapi*.jar $DIR/$tag.backward/java/build/libs

        # prepare the forward compatibility test
        cp -r $tag{,.forward}
        rm -f $DIR/$tag.forward/java/libjavamcsapi.so*
        cp $DIR/../libjavamcsapi.so* $DIR/$tag.forward/java
        rm -f $DIR/$tag.forward/src/libmcsapi.so*
        cp $DIR/../../src/libmcsapi.so* $DIR/$tag.forward/src
    fi
done

# run the tests
echo ""
echo "executing the compatibility tests"
echo ""

cd $DIR
failed=0

for tag in $( git tag -l columnstore-$MAJOR_VERSION.$MINOR_VERSION.* ); do
    #filter versions prior 1.1.2 as these are unstable
    IFS='.' read -r -a CHECK_VERSION <<< ${tag:12}
    if [ ${CHECK_VERSION[0]} -ge 2 -o ${CHECK_VERSION[1]} -ge 2 -o ${CHECK_VERSION[2]} -ge 3 ]; then
        # verify that all original Java tests still pass with the original mcsapi and original javamcsapi (baseline)
        echo "mcsapi: ${tag:12} and javamcsapi: ${tag:12}"
        cd $DIR/$tag/java
        make test
        if [ $? -eq 0 ]; then
            echo "mcsapi: ${tag:12} and javamcsapi: ${tag:12} - PASSED"
        else
            failed=$(($failed+1))
            echo "mcsapi: ${tag:12} and javamcsapi: ${tag:12} - FAILED"
        fi

        # verify that all original Java tests still pass with the original mcsapi and latest javamcsapi (backward compatibility)
        echo "mcsapi: ${tag:12} and javamcsapi: $MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL"
        cd $DIR/$tag.backward/java
        make test
        if [ $? -eq 0 ]; then
            echo "mcsapi: ${tag:12} and javamcsapi: $MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL - PASSED"
        else
            failed=$(($failed+1))
            echo "mcsapi: ${tag:12} and javamcsapi: $MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL - FAILED"
        fi

        # verify that all original Java tests still pass with the latest mcsapi and original javamcsapi (forward compatibility)
        echo "mcsapi: $MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL and javamcsapi: ${tag:12}"
        cd $DIR/$tag.forward/java
        make test
        if [ $? -eq 0 ]; then
            echo "mcsapi: $MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL and javamcsapi: ${tag:12} - PASSED"
        else
            failed=$(($failed+1))
            echo "mcsapi: $MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL and javamcsapi: ${tag:12} - FAILED"
        fi
        echo ""
    fi
done

if [ $failed -eq 0 ]; then
    echo "ALL TESTS PASSED"
else
    echo "$failed TESTS FAILED"
fi

exit $failed
