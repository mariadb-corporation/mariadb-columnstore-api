Building libmcsapi
==================

libmcsapi uses CMake which is a portable cross-platform build system.

Pre-requisites
--------------

You need the development pacakges for **libuv**, **libxml2** and **snappy** to build mcsapi.

To build the documentation you need **python-sphinx** and **python-sphinx-latex**.

The test suite will use **cppcheck** for additional static code analysis checks if it is installed.

Ubuntu
^^^^^^

For the main build you need:

.. code-block:: console

   sudo apt-get install cmake g++ libuv1-dev libxml2-dev libsnappy-dev

For the documentation:

.. code-block:: console

   sudo apt-get install python-sphinx texlive-latex-recommended texlive-latex-extra

For test test suite:

.. code-block:: console

   sudo apt-get install libgtest-dev cppcheck
   cd /usr/src/gtest
   sudo cmake . -DCMAKE_BUILD_TYPE=RELEASE
   sudo make
   sudo mv libg* /usr/lib/

CentOS 7
^^^^^^^^

For the main build you need the following, the devtoolset is because GCC5 minimum is required for full C++11 support:

.. code-block:: console

   sudo yum install cmake libuv-devel libxml2-devel snappy-devel
   sudo yum install centos-release-scl
   sudo yum install devtoolset-4-gcc*
   scl enable devtoolset-4 bash


For the documentation:

.. code-block:: console

   sudo yum install python-sphinx texlive-scheme-full

For the test suite:

.. code-block:: console

   sudo yum install gtest-devel cppcheck

CMake Options
-------------

Several options are available when execution CMake by using the following
command line:

.. code-block:: console

   cmake -D<Variable>=<Value>

Alternatively you can use one of the CMake GUIs to set the options.

The options are as follows:

.. tabularcolumns:: |l|l|p{8cm}|

======================== ==================== =========================================================================================
Option                   Default              Definition
======================== ==================== =========================================================================================
``CMAKE_INSTALL_PREFIX`` (Platform dependent) Where to install libmcsapi
``CMAKE_BUILD_TYPE``     ``RELWITHDEBINFO``   The type of build (``Debug``, ``Release`` or ``RelWithDebInfo``)
``TEST_RUNNER``          ``OFF``              Build the test suite
``BUILD_DOCS``           ``OFF``              Build the PDF documentation
``RPM``                  ``OFF``              Build a RPM (and the OS name for the package)
``DEB``                  ``OFF``              Build a DEB (and the OS name for the package)
``RUN_CPPCHECK``         ``OFF``              Run cppcheck during ``make test`` or ``make all_cppcheck``
======================== ==================== =========================================================================================


Linux / Unix
------------

Requirements
^^^^^^^^^^^^
To compile on POSIX based operating systems you need a functioning C++11 compiler (for GCC version 5.0 minimum) and cmake.  To compile the documentation you will also need python-sphinx version 1.0 or higher.

Compiling
^^^^^^^^^
After running CMake as described above you simple need to run ``make`` and then ``sudo make install``.
To run the test suite you can run ``make check``.

Building a Package
------------------

To build an RPM or DEB package you first need to specify the OS you want to build for, for example:

.. code-block:: console

   cmake . -DRPM=centos7

or

.. code-block:: console

   cmake . -DDEB=xenial

You should of course add options as above to this as required. Then you can build the package using:

.. code-block:: console

   make package
