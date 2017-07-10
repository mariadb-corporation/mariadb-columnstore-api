Usage Introduction
==================

The bulk insert API is designed to provide a very rapid way to insert data into ColumnStore tables. It can be seen as a large transaction which appends data to the end of the column extent files. The commit is atomic by moving the high water mark for the column extents. A rollback is also very fast as it will undo any uncommitted high water mark changes and remove the new extents.

Whilst a bulk insert API is happening a table lock is gained on the table being written to. During this time the table is effectively read-only. Any DML statements are likely to timeout waiting.

It can be run from any point in the ColumnStore installation as it reads the Columnstore.xml file to discover where the PMs are. It can also be run remotely if the Columnstore.xml file is present and the ColumnStore installation was configured using hostnames/IPs accessible by the machine running the API.

Methods in :cpp:class:`ColumnStoreBulkInsert` are not guaranteed to thread safe so an instance of that class should stay in a single thread. But each instance of the class can be run in separate threads.
