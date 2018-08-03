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

**NOTE** Corresponding to your python3 installation, the correct devel packets need to be installed.

For the documentation:

```shell
sudo yum install python34 python34-pip perl
sudo pip3 install -U Sphinx
sudo pip3 install javasphinx
# As CentOS'es LaTeX is broken we need to install texlive from ctan
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

