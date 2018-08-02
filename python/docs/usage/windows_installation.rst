Windows 10 (x64) installation
=============================

After installing the MariaDB Bulk Write SDK in Windows 10 you have to manually add its libraries to your Python (x64) installation to use it.

Following variables will be used to represent the program installation paths needed:

+-------------------------+-----------------------------------------------------------+-----------------------------------------------------------+
| Variable                | Description                                               | Example                                                   |
+=========================+===========================================================+===========================================================+
| ``%PythonInstallDir%``  | The base installation directory Python was installed to   | C:\\Python27                                              |
+-------------------------+-----------------------------------------------------------+-----------------------------------------------------------+
| ``%Python3InstallDir%`` | The base installation directory Python 3 was installed to | C:\\Python37                                              |
+-------------------------+-----------------------------------------------------------+-----------------------------------------------------------+
| ``%mcsapiInstallDir%``  | The base installation directory of the Bulk Write SDK     | C:\\Program Files\\MariaDB\\ColumnStore Bulk Write SDK    |
+-------------------------+-----------------------------------------------------------+-----------------------------------------------------------+

Python 2.7
^^^^^^^^^^
- copy ``libiconv.dll``, ``libuv.dll``, ``libxml2.dll`` and ``mcsapi.dll`` from ``%mcsapiInstallDir%\lib`` into ``%PythonInstallDir%\DLLs``
- copy ``_pymcsapi.pyd`` from  ``%mcsapiInstallDir%\lib\python`` into ``%PythonInstallDir%\DLLs``
- copy ``columnStoreExporter.py``, ``mcsapi_reserved_words.txt`` and ``pymcsapi.py`` from ``%mcsapiInstallDir%\lib\python`` into ``%PythonInstallDir%\Lib``

Python 3.x
^^^^^^^^^^
- copy ``libiconv.dll``, ``libuv.dll``, ``libxml2.dll`` and ``mcsapi.dll`` from ``%mcsapiInstallDir%\lib`` into ``%Python3InstallDir%\DLLs``
- copy ``_pymcsapi.pyd`` from  ``%mcsapiInstallDir%\lib\python3`` into ``%Python3InstallDir%\DLLs``
- copy ``columnStoreExporter.py``, ``mcsapi_reserved_words.txt`` and ``pymcsapi.py`` from ``%mcsapiInstallDir%\lib\python3`` into ``%Python3InstallDir%\Lib``
