ColumnStoreException Class
==========================

.. cpp:class:: ColumnStoreError : public std::runtime_error

   A general execption for the mcsapi classes. It should be used just as the :cpp:class:`std::runtime_error` exception is used. It can be used as a "catchall" for all ColumnStore exceptions

.. cpp:class:: ColumnStoreInternalError : public ColumnStoreError

   An execption class covering problems with the driver's internals

.. cpp:class:: ColumnStoreBufferError : public ColumnStoreError

   An execption class covering problems with the driver's buffers

.. cpp:class:: ColumnStoreServerError : public ColumnStoreError

   An execption class that contains errors received from the server

.. cpp:class:: ColumnStoreNetworkError : public ColumnStoreError

   An execption class covering problems with network connection and communication

.. cpp:class:: ColumnStoreDataError : public ColumnStoreError

   An execption class covering problems with setting column data and data conversion

.. cpp:class:: ColumnStoreUsageError : public ColumnStoreError

   An execption class covering problems with usage of the driver

.. cpp:class:: ColumnStoreConfigError : public ColumnStoreError

   An execption class covering problems with reading the XML configuration

.. cpp:class:: ColumnStoreVersionError : public ColumnStoreError

   An execption class for an incompatible ColumnStore version number

