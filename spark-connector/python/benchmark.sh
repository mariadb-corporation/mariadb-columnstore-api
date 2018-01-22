#!/bin/sh

if [ "$#" -ne 1 ]; then
    echo "$0 path_to_mariadb-java-client.jar"
    exit 2
fi

export PYSPARK_SUBMIT_ARGS="--jars $1 pyspark-shell"

PYTHON_VERSION_AVAILABLE=`python -c "import sys; print(sys.version_info[0]);"`

if [ $PYTHON_VERSION_AVAILABLE -eq 2 ]; then
    PYTHON_2_AVAILABLE=1
    PYTHON_2_EXECUTABLE=`python -c "import sys; print(sys.executable)";`
fi

PYTHON_VERSION_AVAILABLE=`python -c "import sys; print(sys.version_info[0]);"`

if [ $PYTHON_VERSION_AVAILABLE -eq 3 ]; then
    PYTHON_3_AVAILABLE=1
    PYTHON_3_EXECUTABLE=`python -c "import sys; print(sys.executable)";`
fi

PYTHON_VERSION_AVAILABLE=`python3 -c "import sys; print(sys.version_info[0]);"`

if [ $PYTHON_VERSION_AVAILABLE -eq 3 ]; then
    PYTHON_3_AVAILABLE=1
    PYTHON_3_EXECUTABLE=`python3 -c "import sys; print(sys.executable)";`
fi

if [ $PYTHON_2_AVAILABLE -eq 1 ]; then
    export PYSPARK_PYTHON=$PYTHON_2_EXECUTABLE
    export PYSPARK_DRIVER_PYTHON=ipython
    echo "Benchmarking with Python2"
    echo
    $PYTHON_2_EXECUTABLE test/benchmark.py
    echo
    echo
fi

if [ $PYTHON_3_AVAILABLE -eq 1 ]; then
    export PYSPARK_PYTHON=$PYTHON_3_EXECUTABLE
    export PYSPARK_DRIVER_PYTHON=ipython3
    echo "Benchmarking with Python3"
    echo
    $PYTHON_3_EXECUTABLE test/benchmark.py
fi
