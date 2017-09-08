# MariaDB ColumnStore API

**WARNING**
This is a work-in-progress tree for the MariaDB ColumnStore API. *It should currently be considered alpha quality.*

It requires MariaDB ColumnStore 1.1 (current develop branch) to operate.

This C++ API currently provides access to bulk write to ColumnStore in a similar way to the cpimport using.

## Bugs

Please file bugs using the [MariaDB ColumnStore Jira](https://jira.mariadb.org/browse/MCOL) 

## Building

### Ubuntu

For the main build you need:

```shell
sudo apt-get install cmake g++ libuv1-dev libxml2-dev libsnappy-dev pkg-config
```

For the documentation:

```shell
sudo apt-get install python-sphinx texlive-latex-recommended texlive-latex-extra
```

For test test suite:

```shell
sudo apt-get install libgtest-dev cppcheck
cd /usr/src/gtest
sudo cmake . -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON
sudo make
sudo mv libg* /usr/lib/
```

### CentOS 7

For the main build you need the following, the devtoolset is because GCC5 minimum is required for full C++11 support:

```shell
sudo yum install cmake libuv-devel libxml2-devel snappy-devel
sudo yum install centos-release-scl
sudo yum install devtoolset-4-gcc*
scl enable devtoolset-4 bash
```

For the documentation:

```shell
sudo yum install python-sphinx texlive-scheme-full latexmk
```

For the test suite:

```shell
sudo yum install gtest-devel cppcheck
```

### CMake Options

Several options are available when execution CMake by using the following
command line:

```shell
cmake -D<Variable>=<Value>
```

Alternatively you can use one of the CMake GUIs to set the options.

The options are as follows:

| Option | Default | Definition |
| ------ | ------ | ---------- |
| ``CMAKE_INSTALL_PREFIX`` | (Platform dependent) | Where to install libmcsapi |
| ``CMAKE_BUILD_TYPE`` | ``RELWITHDEBINFO`` | The type of build (``Debug``, ``Release`` or ``RelWithDebInfo``) |
| ``TEST_RUNNER`` | ``OFF`` | Build the test suite |
| ``BUILD_DOCS`` | ``OFF`` | Build the PDF documentation |
| ``RPM`` | ``OFF`` | Build a RPM (and the OS name for the package) |
| ``DEB`` | ``OFF`` | Build a DEB (and the OS name for the package) |
| ``RUN_CPPCHECK`` | ``OFF`` | Run cppcheck during ``make test`` or ``make all_cppcheck``|

### Compiling

After running CMake as described above you simple need to run ``make`` and then ``sudo make install``.
To run the test suite you can run ``make check``.

## Building a Package

To build an RPM or DEB package you first need to specify the OS you want to build for, for example:

```shell
cmake . -DRPM=centos7
```

```shell
cmake . -DDEB=xenial
```

You should of course add options as above to this as required. Then you can build the package using:

```shell
make package
```
