Windows 10 (x64) installation
=============================

If Python 2.7 or Python 3.7 is detected during pymcsapi's installation, it can be installed automatically.

Manual Installation
-------------------

If Python 2.7 or Python 3.7 wasn't detected during pymcsapi's installation you can install pymcsapi manually afterwards.

Following variables will be used to represent the program installation paths needed:

.. tabularcolumns:: |p{3.8cm}|p{6cm}|p{6.2cm}|

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
- copy ``columnStoreExporter.py``, ``mcsapi_reserved_words.txt`` and ``pymcsapi.py`` from ``%mcsapiInstallDir%\lib\python2.7`` into ``%PythonInstallDir%\Lib``

Python 3.7
^^^^^^^^^^
- copy ``libiconv.dll``, ``libuv.dll``, ``libxml2.dll`` and ``mcsapi.dll`` from ``%mcsapiInstallDir%\lib`` into ``%Python3InstallDir%\DLLs``
- copy ``_pymcsapi.pyd`` from  ``%mcsapiInstallDir%\lib\python3`` into ``%Python3InstallDir%\DLLs``
- copy ``columnStoreExporter.py``, ``mcsapi_reserved_words.txt`` and ``pymcsapi.py`` from ``%mcsapiInstallDir%\lib\python3.7`` into ``%Python3InstallDir%\Lib``

**Note:** Unlike the Linux version of pymcsapi3 the Windows version of pymcsapi3 can only be used with the Python 3 release it was compiled with. Swig for Windows compiles against the Python3x.lib and not the main Python3.lib. Therefore, you might have to recompile pymcsapi3 from scratch if you need to use a specific Python 3 release.
