# MariaDB ColumnStore API

**WARNING**
This is a work-in-progress tree for the MariaDB ColumnStore API. *It should currently be considered pre-alpha quality.*

It requires MariaDB ColumnStore branch MCOL-769 to operate.

This C++ API currently provides access to bulk write to ColumnStore in a similar way to the cpimport using.

## Bugs

Please file bugs using the [MariaDB ColumnStore Jira](https://jira.mariadb.org/browse/MCOL) 

## Building

###Ubuntu
For the main build you need:

```console
sudo apt-get install cmake g++ libuv1-dev libxml2-dev libsnappy-dev
```
For the documentation:
```console
sudo apt-get install python-sphinx texlive-latex-recommended texlive-latex-extra
```

For test test suite:

```console
sudo apt-get install libgtest-dev cppcheck
cd /usr/src/gtest
sudo cmake . -DCMAKE_BUILD_TYPE=RELEASE
sudo make
sudo mv libg* /usr/lib/
```
### CentOS 7

For the main build you need

```console
sudo yum install cmake gcc-c++ libuv-devel libxml2-devel snappy-devel
```

For the documentation:

```console
sudo yum install python-sphinx texlive-scheme-full
```

For the test suite:
```console
sudo yum install gtest-devel cppcheck
```

### CMake Options

Several options are available when execution CMake by using the following
command line:

```console
cmake -D<Variable>=<Value>
```

Alternatively you can use one of the CMake GUIs to set the options.

The options are as follows:

| Option | Default | Definition |
| ------ | ------ | ---------- |
| ``CMAKE_INSTALL_PREFIX`` | (Platform dependent) | Where to install libmcsapi |
| ``CMAKE_BUILD_TYPE`` | ``RELWITHDEBINFO`` | The type of build (``Debug``, ``Release`` or ``RelWithDebInfo``) |
| ``TEST_RUNNER`` | ``OFF`` | Build the test suite |
| ``BUILD_DOCS`` | ``OFF`` | Build the HTML documentation |
| ``PDFLATEX_COMPILER`` | ``OFF`` | Build the PDF documentation (requires ``BUILD_DOCS=ON``) |
| ``RUN_CPPCHECK`` | ``OFF`` | Run cppcheck during ``make test`` or ``make all_cppcheck``|

### Compiling
After running CMake as described above you simple need to run ``make`` and then ``sudo make install``.
To run the test suite you can run ``make check``.
