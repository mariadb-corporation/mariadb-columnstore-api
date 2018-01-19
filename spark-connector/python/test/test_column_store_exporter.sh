#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "$! python_executable_path pyspark_driver_python"
	exit 2
fi

export PYSPARK_PYTHON=$1
export PYSPARK_DRIVER_PYTHON=$2
$1 -m pytest test/test_column_store_exporter.py
