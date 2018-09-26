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
sudo apt-get install python python-pip python3 python3-pip texlive-latex-recommended texlive-latex-extra latexmk
sudo pip2 install -U Sphinx
sudo pip2 install javasphinx
sudo pip3 install -U Sphinx
sudo pip3 install javasphinx
```

For test test suite:

```shell
sudo apt-get install libgtest-dev cppcheck default-libmysqlclient-dev
cd /usr/src/gtest
sudo cmake . -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON
sudo make
sudo mv libg* /usr/lib/
# from the cloned mariadb-columnstore-api directory
pip2 install --no-cache-dir -r spark-connector/python/test/requirements.txt
pip3 install --no-cache-dir -r spark-connector/python/test/requirements.txt
```
### Debian 8 (Jessie)

Debian Jessie requires packages that are not in the main repositories. First of all you need Debian's Jessie backports repository enabled, edit the file `/etc/apt/sources.list` and add the following line:

```
deb http://httpredir.debian.org/debian jessie-backports main contrib non-free
```

Then install the following:

```shell
sudo apt-get install cmake g++ libuv1-dev libxml2-dev libsnappy-dev pkg-config libc++-dev python-dev python3-dev libboost-dev libpcre++-dev
# As Debian 8's swig is too old and causes errors we need to install swig from source
wget https://iweb.dl.sourceforge.net/project/swig/swig/swig-3.0.12/swig-3.0.12.tar.gz
tar -xf swig-3.0.12.tar.gz
cd swig-3.0.12/
./configure
make
sudo make install
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
sudo apt-get install python python-pip python3 python3-pip texlive-latex-recommended texlive-latex-extra latexmk
sudo pip2 install --upgrade pip
sudo pip3 install --upgrade pip
sudo pip2 install -U Sphinx
sudo pip2 install javasphinx
sudo pip3 install -U Sphinx
sudo pip3 install javasphinx
```

For the test suite do the following in a directory separate from the API:

```shell
sudo apt-get install cppcheck libmysqlclient-dev
git clone https://github.com/google/googletest
cd googletest
cmake . -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_SHARED_LIBS=ON
make
sudo make install
# from the cloned mariadb-columnstore-api directory
sudo pip2 install --upgrade wheel
sudo pip3 install --upgrade wheel
sudo pip2 install --no-cache-dir -r spark-connector/python/test/requirements.txt
sudo pip3 install --no-cache-dir -r spark-connector/python/test/requirements.txt
```

### CentOS 7

For the main build you need the following:

```shell
sudo yum install epel-release
sudo yum install cmake libuv-devel libxml2-devel snappy-devel gcc-c++
sudo yum install java-1.8.0-openjdk java-1.8.0-openjdk-devel python-devel python34-devel boost-devel pcre-devel
# As CentOS'es swig is too old and causes errors we need to install swig from source
curl -O https://iweb.dl.sourceforge.net/project/swig/swig/swig-3.0.12/swig-3.0.12.tar.gz
tar -xf swig-3.0.12.tar.gz
cd swig-3.0.12/
./configure
make
sudo make install
```

**NOTES** -
- If you already have the os version of swig installed, remove it first with yum remove swig.
- Corresponding to your python3 installation, the correct devel packets need to be installed.

For the documentation:

```shell
sudo yum install python python-pip python34 python34-pip perl perl-Digest-MD5
sudo pip3 install -U Sphinx
sudo pip3 install javasphinx
# As CentOS'es LaTeX is broken we need to install texlive from ctan
curl -L -O http://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz
tar -xf install-tl-unx.tar.gz
cd install-tl-*
sudo ./install-tl << EOF
O
L
/usr/sbin


R
I
EOF
```

For the test suite:

```shell
sudo yum install gtest-devel cppcheck mariadb-devel
# from the cloned mariadb-columnstore-api directory
sudo pip2 install --upgrade pip
sudo pip3 install --upgrade pip
sudo pip2 install --upgrade wheel
sudo pip3 install --upgrade wheel
sudo pip2 install --no-cache-dir -r spark-connector/python/test/requirements.txt
sudo pip3 install --no-cache-dir -r spark-connector/python/test/requirements.txt
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

## Windows 10 (x64) [Alpha]

Currently only the documentation can't be built on Windows.

mcsapi requires the [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) (x64) to be executable.

### Build dependencies

For the main build you need:  
- [Microsoft Visual Studio 2017](https://visualstudio.microsoft.com/downloads/) (the Community Edition is sufficient)  
  the option "Desktop development with C++" is needed
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) \[[download](http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/pkg-config_0.26-1_win32.zip)\]
- [cmake](https://cmake.org/)
- [boost](https://www.boost.org/) > 1.58.0.0 \[headers only\]
- [libiconv](https://savannah.gnu.org/projects/libiconv/) \[shared\]
- [libxml2](https://gitlab.gnome.org/GNOME/libxml2/) \[shared\]
- [libuv](https://github.com/libuv/libuv) \[shared\]
- [snappy](https://github.com/google/snappy) \[static\]

For the Java API you need in addition:  
- [Java SDK 8 (x64)](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)
- [swig](http://www.swig.org/download.html)

For the Python API you need in addition:  
- [Python 2.7 (x64)](https://www.python.org/downloads/windows/)
- [Python 3 (x64)](https://www.python.org/downloads/windows/)
- [swig](http://www.swig.org/download.html)

In order to build packets for Python 2 and Python 3, Python 3's executable needs to be manually renamed from ``python.exe`` to ``python3.exe``. Install both Python releases directly under ``C:\``.  
For testing it is required to install the modules ``pytest``, ``pyspark`` and ``mysql-connector``.

For the test suite you need in addition:  
- [googletest](https://github.com/google/googletest) \[static\]
- [libmysql](https://dev.mysql.com/downloads/connector/c/) \[shared\]

And for the package build you need in addition:  
- [WiX toolset](http://wixtoolset.org/)
- Set the environment variable ``WIX`` to the WiX installation directory

**You can either compile all dependent C++ libraries (64bit) by yourself according to our documentation further below, or can [download](https://drive.google.com/a/mariadb.com/file/d/1J9lQ_ddEKlYaReFH6hgkiLaixnBnVrdi/view?usp=sharing) a pre-compiled collection of the dependent libraries. Once you have obtained (and extracted) the dependent libraries you have to set the environment variable `MCSAPI_BUILD_DEPENDENCY_DIR` to the directory containing the `include` and `lib` sub-directories. (e.g. to `C:\mcsapi-windows-library-collection`)

**NOTE**  
Please ensure that all tools are executable from command line and have a valid ``Path`` entry.

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

### Signed package build

To create a signed msi install package you first need to import MariaDB private sign certificate. Afterwards you can use following commands:
```
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api.git
cd mariadb-columnstore-api
git checkout develop-1.1
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config RelWithDebInfo --target package
signtool.exe sign /tr http://timestamp.digicert.com /td sha256 /fd sha256 /a "MariaDB ColumnStore Bulk Write SDK-*-x64.msi"
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
- pymcsapi3 depends on the Python 3 DLL of the Python release used to compile pymcsapi3. Therefore, pymcsapi3 is unlike the Linux version not yet Python 3 release independent.
- pymcsapi3's spark-connector tests fail if Python3 is not installed directly under ``C:\``.

### Dependent C++ library compilation / acquisition
This section describes how the dependent C++ libraries necessary for build and test were obtained and bundled into an [archive](https://drive.google.com/a/mariadb.com/file/d/1J9lQ_ddEKlYaReFH6hgkiLaixnBnVrdi/view?usp=sharing) for easy reuseability.

#### Boost 1.67.0.0
The boost headers were obtained through Visual Studio's packet manager [nuget](https://www.nuget.org/downloads).
```
nuget.exe --install boost
```
Include file location: ``boost.1.67.0.0\lib\native\include``

#### libiconv 1.15 (shared)
The shared libiconv library was compiled with Visual Studio 2017.
```
git clone https://github.com/pffang/libiconv-for-Windows
cd libiconv-for-Windows
git checkout 837de0484f1af21517575f22652e04702e10fcdb
msbuild /t:Clean
```
- Open LibIconv.sln with Visual Studio 2017
- Change target from ``Debug`` to ``Release``
- Change platform from ``Win32`` to ``x64``
- Project --> Properties --> Windows SDK Version --> 10.0.17134.0
- Project --> Properties --> Platform toolset --> Visual Studio 2017 (v141)
- Apply --> OK
- Build --> Rebuild Solution

Include file location: ``libiconv-for-Windows\include``  
Library location: ``libiconv-for-Windows\lib64``  
Shared library location: ``libiconv-for-Windows\lib64``

#### libxml2 2.9.8 (shared)
The shared libxml2 library was compiled with Visual Studio 2017 and needed above mentioned libiconv libraries as dependencies.
```
git clone https://gitlab.gnome.org/GNOME/libxml2.git
cd libxml2
git checkout v2.9.8
cd win32
# copy libiconv.lib from libiconv-for-Windows\lib64 into libxml2\win32 and rename it to iconv.lib
# copy iconv.h from libiconv-for-Windows\include into libxml2\win32
cscript configure.js
nmake /f Makefile.msvc
```
Include file location: ``libxml2\win32``  
Library location: ``libxml2\win32\bin.msvc``  
Shared library location: ``libxml2\win32\bin.msvc``

#### libuv 1.22.0 (shared)
The shared libuv library was compiled with Visual Studio 2017. Python 2.7 is needed for the build.
```
git clone https://github.com/libuv/libuv.git
cd libuv
git checkout v1.22.0
vcbuild.bat vs2017 release x64 shared test
```
Include file location: ``libuv\include``  
Library location: ``libuv\Release``  
Shared library location: ``libuv\Release``

#### snappy 1.1.7 (static)
The static snappy library was compiled with Visual Studio 2017.
```
git clone https://github.com/google/snappy.git
cd snappy
git checkout 1.1.7
mkdir build && cd build
cmake .. -G "Visual Studio 15 2017 Win64"
cmake --build . --config RelWithDebInfo
```
Include file locations: ``snappy`` and ``snappy\build``  
Library location: ``snappy\build\RelWithDebInfo``

#### google test \[master\] (static)
The static google test libraries were compiled with Visual Studio 2017.
```
git clone https://github.com/google/googletest.git
cd googletest
git checkout 6b6be9457bcec4540bd9533c9c282dfc2de1e81d
mkdir build && cd build
cmake .. -G "Visual Studio 15 2017 Win64" -Dgtest_force_shared_crt=ON
cmake --build . --config RelWithDebInfo
```
Include file locations: ``googletest\googlemock\include`` and ``googletest\googletest\include``  
Library locations: ``googletest\build\googlemock\RelWithDebInfo`` and ``googletest\build\googlemock\gtest\RelWithDebInfo``

#### libmysql 6.1 (shared)
The shared libmysql libraries were obtained directly from Oracle. Using the [MySQL Installer 8.0.12](https://dev.mysql.com/downloads/installer/) the MySQL Connector C libraries of version 6.1 were installed to the system.

Include file location: ``C:\Program Files\MySQL\MySQL Connector C 6.1\include``  
Library locations: ``C:\Program Files\MySQL\MySQL Connector C 6.1\lib`` and ``C:\Program Files\MySQL\MySQL Connector C 6.1\lib\vs14``
