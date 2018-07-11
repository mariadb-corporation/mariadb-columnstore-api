Compiling with javamcsapi
=========================

Pre-requisites
--------------

To link javamcsapi to your application and to execute it you first need install the following mcsapi pre-requisites:

Ubuntu
^^^^^^

.. code-block:: console

   sudo apt-get install libuv1

CentOS 7
^^^^^^^^

.. code-block:: console

   sudo yum install epel-release
   sudo yum install libuv

Compiling
---------

To compile a Java program from command line you have to explicitly state where to find javamcsapi.jar. Its path can differ depending on your operating system.

Ubuntu
^^^^^^

.. code-block:: console

   javac -classpath ".:/usr/lib/javamcsapi.jar" Basic_bulk_insert.java
   java -classpath ".:/usr/lib/javamcsapi.jar" Basic_bulk_insert

CentOS 7
^^^^^^^^

.. code-block:: console

   javac -classpath ".:/usr/lib64/javamcsapi.jar" Basic_bulk_insert.java
   java -classpath ".:/usr/lib64/javamcsapi.jar" Basic_bulk_insert

