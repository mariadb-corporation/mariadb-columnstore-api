# MariaDB ColumnStore API

The API requires MariaDB ColumnStore 1.1 to operate.

This C++, Python and Java API currently provides access to bulk write to ColumnStore in a similar way to the cpimport using.

## Bugs

Please file bugs using the [MariaDB ColumnStore Jira](https://jira.mariadb.org/browse/MCOL) 

## Build Dependencies

**NOTE**
CentOS 6 is not currently supported and it is not expected that the API will build on this platform.

### Ubuntu 16.04 (Xenial) / Debian 9 (Stretch)

For the main build you need:

```shell
sudo apt-get install cmake g++ libuv1-dev libxml2-dev libsnappy-dev pkg-config swig python-dev python3-dev default-jdk libboost-dev
```

For the documentation:

```shell
sudo apt-get install python3 python3-pip texlive-latex-recommended texlive-latex-extra latexmk
pip3 install -U Sphinx
pip3 install javasphinx
```

For test test suite:

```shell
sudo apt-get install libgtest-dev cppcheck default-libmysqlclient-dev
cd /usr/src/gtest
sudo cmake . -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON
sudo make
sudo mv libg* /usr/lib/
```
### Debian 8 (Jessie)

Debian Jessie requires packages that are not in the main repositories. First of all you need Debian's Jessie backports repository enabled, edit the file `/etc/apt/sources.list` and add the following line:

```
deb http://httpredir.debian.org/debian jessie-backports main contrib non-free
```

Then install the following:

```shell
sudo apt-get install cmake g++ libuv1-dev libxml2-dev libsnappy-dev pkg-config libc++-dev swig python-dev python3-dev libboost-dev
```

A JavaSDK >= 8 is required to run properly. If not installed do the following:
```shell
sudo apt-get install -t jessie-backports openjdk-8-jdk
```
If more than one JavaSDK is installed, change the default to >= 8 by:
```shell
sudo update-alternatives --config java
```

For the documentation:

```shell
sudo apt-get install python3 python3-pip texlive-latex-recommended texlive-latex-extra latexmk
pip3 install -U Sphinx
pip3 install javasphinx
```

For the test suite do the following in a directory separate from the API:

```shell
sudo apt-get install cppcheck libmysqlclient-dev
git clone https://github.com/google/googletest
cd googletest
cmake . -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON
make
sudo make install
```

### CentOS 7

For the main build you need the following:

```shell
sudo yum install epel-release
sudo yum install cmake libuv-devel libxml2-devel snappy-devel
sudo yum install java-1.8.0-openjdk java-1.8.0-openjdk-devel swig python-devel python34-devel boost-devel
```

**NOTE** Corresponding to your python3 installation, the correct devel packets need to be installed.

For the documentation:

```shell
sudo yum install python34 python34-pip perl
sudo pip3 install -U Sphinx
sudo pip3 install javasphinx
# As CentOS's LaTeX is broken we install texlive from ctan
curl -L -O http://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz
tar -xf install-tl-unx.tar.gz
cd install-tl-*
sudo ./install-tl << EOF
O
L



R
I
EOF
```

For the test suite:

```shell
sudo yum install gtest-devel cppcheck mariadb-devel
```

### SUSE Enterprise Linux 12

For the main build you need GCC5 minimum. For this example we are using GCC6, you will need the SDK and Toolchain modules enabled in Yast first:

```shell
sudo zypper install gcc6 gcc6-c++ cmake libxml2-devel snappy-devel git boost-devel

export CC=/usr/bin/gcc-6
export CXX=/usr/bin/g++-6
```

Then in a directory separate from the API:

```shell
git clone https://github.com/libuv/libuv
cd libuv
./autogen.sh
./configure
make
sudo make install
```

Unfortunately it is not possible to build the documentation in SUSE Enterprise Linux 12 due to missing LaTeX dependencies.

For the test suite do the following in a directory separate from the API:

```shell
sudo zypper ar -f http://download.opensuse.org/repositories/devel:/tools/SLE_12_SP3/devel:tools.repo
sudo zypper install cppcheck
git clone https://github.com/google/googletest
cmake . -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON
make
sudo make install
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
| ``PYTHON`` | ``ON`` | Build the Python library |
| ``JAVA`` | ``ON`` | Build the Java library |
| ``SPRK_CONNECTOR`` | ``ON`` | Build the spark-connector library for Python and Scala |
| ``RUN_CPPCHECK`` | ``OFF`` | Run cppcheck during ``make test`` or ``make all_cppcheck``|

### Compiling

After running CMake as described above you simple need to run ``make`` and then ``sudo make install``.
To run the test suite you can run ``make check``.

## Building a Package

In addition to the normal dependencies, the following packages need to be
installed for packaging.

### CentOS/RHEL and SLES

```
sudo yum -y install rpm-build
```

### Debian and Ubuntu (all versions)

```
sudo apt-get -y install dpkg-dev
```

To build an RPM or DEB package you first need to specify the OS you want to build for, for example:

```shell
cmake . -DRPM=centos7
```

```shell
cmake . -DDEB=xenial
```

You should of course add options as above to this as required. Then you can build the package using:

```shell
sudo make package
```

## Windows 10 (x64) [EXPERIMENTAL]

Currently only the documentation can't be built on Windows.

### Build dependencies

For the main build you need:

- [Microsoft Visual Studio 2017](https://visualstudio.microsoft.com/downloads/) (the Community Edition is sufficient)
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [cmake](https://cmake.org/)
- [boost](https://www.boost.org/) > 1.58.0.0
- [libiconv](https://savannah.gnu.org/projects/libiconv/)
- [libxml2](https://gitlab.gnome.org/GNOME/libxml2/)
- [libuv](https://github.com/libuv/libuv)
- [snappy](https://github.com/google/snappy)

For the Java API you need in addition:

- [Java SDK 8 (x64)](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)
- [swig](http://www.swig.org/download.html)

For the Python API you need in addition:

- [Python 2.7 (x64)](https://www.python.org/downloads/windows/)
- [Python 3 (x64)](https://www.python.org/downloads/windows/)
- [swig](http://www.swig.org/download.html)

In order to build packets for Python 2 and Python 3, Python 3's executable needs to be manually renamed from ``python.exe`` to ``python3.exe``.
For testing it is required to install the modules ``pytest``, ``pyspark`` and ``mysql-connector``.

For the test suite you need in addition:

- [googletest](https://github.com/google/googletest)
- [libmysql](https://dev.mysql.com/downloads/connector/c/)

And for the package build you need in addition:

- [WiX toolset](http://wixtoolset.org/)
- Set the environment variable ``WIX`` to the WiX installation directory

**Compile all libraries for 64bit and add them to your Visual Studio installation. Don't forget to add the runtime libraries (dlls) as well.**

**NOTE** Please ensure that all tools are executable from command line and have a valid ``Path`` entry.

### Compiling

To compile mcsapi enter following commands in x64 Native Tools Command Prompt for VS 2017.

```
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api.git
cd mariadb-columnstore-api
git checkout develop-1.1
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config RelWithDebInfo
```

### Package build

To create a msi install package use following commands:
```
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api.git
cd mariadb-columnstore-api
git checkout develop-1.1
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config RelWithDebInfo --target package
```

### Testing
1) Create a Columnstore.xml file according to our [Knowledge Base](https://mariadb.com/kb/en/library/columnstore-bulk-write-sdk/#environment-configuration) pointing to the ColumnStore instance you want to use for testing and place it into an ``etc`` directory
2) Set the environment variable ``COLUMNSTORE_INSTALL_DIR`` to the folder holding the ``etc`` directory
3) Create a remote user and grant him access to the mcsapi and infinidb_vtable databases on the ColumnStore instance you want to use for testing
4) Set the environment variable ``MCSAPI_CS_TEST_IP`` to the IP address of the ColumnStore instance you want to use for testing (on the Windows machine)
5) Set the environment variables ``MCSAPI_CS_TEST_USER`` and ``MCSAPI_CS_TEST_PASSWORD`` to hold the credentials of the newly created remote user (on the Windows machine)
6) Compile mcsapi as described below with the additional ``TEST_RUNNER`` option and invoke the tests with ``ctest``. If you would like to test pymcsapi, you have to install it in your python environment(s) before the test execution.
```
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api.git
cd mariadb-columnstore-api
git checkout develop-1.1
mkdir build && cd build
cmake -DTEST_RUNNER=ON -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config RelWithDebInfo
ctest -C RelWithDebInfo
```

### Known limitations
- Javamcsapi's test suite can currently only be executed from the top level ctest by a user without special characters. Users whose names contain special characters need to execute ctest manually from the build/java directory to test javamcsapi.
- The debug build contains the whole path of the debug file instead of only its file name.
