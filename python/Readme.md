# MariaDB ColumnStore API Python Wrapper
This provides a python wrapper around the C++ MariaDB ColumnStore API. It takes advantage of [pybind11](https://github.com/pybind/pybind11) to provide an almost identical interface to the C++ SDK but in python.

# Building
Python must be installed on the server and in the path (currently tested with Python 2.x). In addition the following dependencies on top of mcsapi should be installed (tested on Centos 7) for building:
```sh
$ sudo yum install python-devel
```

and for testing:
```sh
$ sudo pip install mysql-connector pytest
```

To build requires enabling the cmake option PYTHON from the mariadb-columnstore-api directory:
```sh
$ cmake -DPYTHON=on
$ make
```

This should download and install the pybind11 source tree required for compilation and then compile.

# Testing
The unit tests can be run with pytest from the python directory:
```sh
$ pytest test.py
```

In addition a simple reference implement of python cpimport is provided as cpimport.py taking three arguments:
```sh
$ python cpimport.py test mytab mytab.tbl
```
