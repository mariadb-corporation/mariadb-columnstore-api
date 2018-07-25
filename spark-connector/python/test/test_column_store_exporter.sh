#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "$0 python_executable_path pyspark_driver_python"
    exit 666
fi

SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

export PYSPARK_PYTHON=$1
export PYSPARK_DRIVER_PYTHON=$2
$1 -m pytest $SCRIPTPATH/test_column_store_exporter.py
if [ $? -ne 0 ]; then
    exit 1
fi
$1 -m pytest $SCRIPTPATH/test_column_store_SQL_generation.py
if [ $? -ne 0 ]; then
    exit 2
fi
