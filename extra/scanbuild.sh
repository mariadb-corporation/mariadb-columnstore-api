#!/bin/bash

# Script to run clang static code analysis

CC="clang"
CXX="clang++"
cmake . -DCMAKE_BUILD_TYPE=Debug -DTEST_RUNNER=ON
scan-build --use-cc=clang --use-c++=clang++ --status-bugs make
