ColumnStoreException Class
==========================

.. cpp:class:: ColumnStoreException : public std::runtime_error

   A general execption for the mcsapi classes. It should be used just as the :cpp:class:`std::runtime_error` exception is used.

.. cpp:class:: ColumnStoreDriverException : public ColumnStoreException

   An execption class covering problems occuring with the :cpp:class:`ColumnStoreDriver` class.
