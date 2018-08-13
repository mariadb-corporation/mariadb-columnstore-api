Compiling with mcsapi
=====================

Pre-requisites
--------------

To link mcsapi to your application you first need install the following pre-requisites:

Ubuntu
^^^^^^

.. code-block:: console

   sudo apt-get install libsnappy1v5 libuv1 libxml2 g++ gcc pkg-config libbost-dev

CentOS 7
^^^^^^^^

.. code-block:: console

   sudo yum install snappy libuv libxml2 pkgconfig boost-devel gcc gcc-c++


Windows 10 (x64)
^^^^^^^^^^^^^^^^

On Windows you need to install the `Visual C++ Redistributable for Visual Studio 2015`_ in the x64 version in order to use mcsapi.
You have to install Visual Studio 2017 to compile .cpp files that use mcsapi.

Compiling
---------

Linux
^^^^^

The easiest way to compile is to use pkg-config to provide the required compile options.

The following is a basic example of how to do compile an example c++ application with mcsapi:

.. code-block:: console

   g++ example.cpp -o example -std=c++11 `pkg-config libmcsapi --cflags --libs`


Windows 10 (x64)
^^^^^^^^^^^^^^^^
Here is an basic example using Visual Studio's x64 Native Tools Command Prompt:

.. code-block:: console

   cl example.cpp %mcsapiInstallDir%\lib\mcsapi.lib /I %mcsapiInstallDir%\include

The variable ``%mcsapiInstallDir%`` represents the base installation directory of the Bulk Write SDK. (e.g. C:\\Program Filesi\\MariaDBi\\ColumnStore Bulk Write SDK)

To run the built executable you have to copy mcapi's DLLs ``libiconv.dll``, ``libuv.dll``, ``libxml2.dll`` and ``mcsapi.dll`` from ``%mcsapiInstallDir%\lib`` to the directory of the built executable.

.. _`Visual C++ Redistributable for Visual Studio 2015`: https://www.microsoft.com/en-us/download/details.aspx?id=48145
