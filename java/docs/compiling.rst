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


Windows 10 (x64)
^^^^^^^^^^^^^^^^

.. code-block:: console

   javac -classpath ".;%mcsapiInstallDir%\lib\java\javamcsapi-1.1.7.jar" Basic_bulk_insert.java
   java -classpath ".;%mcsapiInstallDir%\lib\java\javamcsapi-1.1.7.jar" -Djava.library.path="%mcsapiInstallDir%\lib" Basic_bulk_insert

The variable ``%mcsapiInstallDir%`` represents the base installation directory of the Bulk Write SDK. (e.g. ``C:\Program Files\MariaDB\ColumnStore Bulk Write SDK``)

If you don't want to change the ``java.library.path`` you can copy javamcapi's DLLs ``libiconv.dll``, ``libuv.dll``, ``libxml2.dll``, ``mcsapi.dll`` and ``javamcsapi.dll`` from ``%mcsapiInstallDir%\lib`` to the directory of the Java class to execute.  
Another option is to just add ``%mcsapiInstallDir%\lib`` to your ``PATH`` environment variable, which is the default setting when you install the Bulk Write SDK.
