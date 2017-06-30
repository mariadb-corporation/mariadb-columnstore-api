Compiling with mcsapi
=====================

Pre-requisites
--------------

To link mcsapi to your application you first need install the following pre-requisites:

Ubuntu
^^^^^^

.. code-block:: console

   $ sudo apt-get install libsnappy1v5 libuv1 libxml2 g++ gcc pkg-config

CentOS 7
^^^^^^^^

.. code-block:: console

   $ sudo yum install snappy libuv libxml2 gcc-c++ gcc pkgconfig

Compiling
---------

The easiest way to compile is to use pkg-config to provide the required compile options.

The following is a basic example of how to do compile an example c++ application with mcsapi:

.. code-block:: console

   $ g++ example.cpp -o example `pkg-config libmcsapi --cflags --libs`
