#!/bin/python

# Copyright (c) 2018 , MariaDB Corporation. All rights reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA

import pytest, datetime
import mysql.connector as mariadb
import pymcsapi, os
from six.moves import range

DB_NAME = 'mcsapi'

def initialize_connection_variables():
    global user
    user = "root"
    global host
    host = "localhost"
    if os.environ.get("MCSAPI_CS_TEST_IP") is not None:
        host=os.environ.get("MCSAPI_CS_TEST_IP")
    if os.environ.get("MCSAPI_CS_TEST_USER") is not None:
        user=os.environ.get("MCSAPI_CS_TEST_USER")
    global password
    password = os.environ.get("MCSAPI_CS_TEST_PASSWORD")

def create_db():
    try:
        conn = mariadb.connect(user=user, password=password, host=host)
        cursor = conn.cursor();
        cursor.execute("CREATE DATABASE IF NOT EXISTS %s" %(DB_NAME,))
    except mariadb.Error as err:
        pytest.fail("Error creating database %s" %(err,))
    finally:
        if cursor: cursor.close()
        if conn: conn.close()

def create_conn():
    initialize_connection_variables()
    create_db()    
    try:
        return mariadb.connect(user=user, password=password, host=host, database=DB_NAME)
    except mariadb.Error as err:
        pytest.fail("Error connecting to mcsapi database %s" %(err,))        

def exec_stmt(conn, stmt):
    try:
        cursor = conn.cursor()
        cursor.execute(stmt)
    except mariadb.Error as err:
        pytest.fail("Error executing statement: %s, error: %s" %(stmt,err))
    finally:
        if cursor: cursor.close()

def drop_table(conn, tablename):
    exec_stmt(conn, "DROP TABLE IF EXISTS %s" %(tablename,))
    
    
def test_list_table_locks():
    conn = create_conn()
    tablename1 = 'pymcsapi_lock1'
    tablename2 = 'pymcsapi_lock2'
    drop_table(conn, tablename1)
    drop_table(conn, tablename2)
    exec_stmt(conn, 'CREATE TABLE %s (a int, b int) engine=columnstore' % tablename1)
    exec_stmt(conn, 'CREATE TABLE %s (a int, b int) engine=columnstore' % tablename2)
   
    d = pymcsapi.ColumnStoreDriver()

    try:
        lockedTablesAtBeginning = len(d.listTableLocks())
        b1 = d.createBulkInsert(DB_NAME, tablename1, 0, 0)
        # verify that one additional table is locked
        assert 1 == (len(d.listTableLocks()) - lockedTablesAtBeginning)
        b2 = d.createBulkInsert(DB_NAME, tablename2, 0, 0)
        # verify that two additional tables are locked
        assert 2 == (len(d.listTableLocks()) - lockedTablesAtBeginning)
        b2.rollback()
        # verify that one additional table is locked
        assert 1 == (len(d.listTableLocks()) - lockedTablesAtBeginning)
        b1.rollback()
        assert 0 == (len(d.listTableLocks()) - lockedTablesAtBeginning)
    except RuntimeError as err:
        pytest.fail("Error executing driver operation: %s" % (err,)) 
 
    try:
        drop_table(conn, tablename1)
        drop_table(conn, tablename2)
    except mariadb.Error as err:
        pytest.fail("Error during cleanup: %s" %(err,))
    finally:  
        conn.close()
    
def test_is_table_locked():
    conn = create_conn()
    tablename = 'pymcsapi_lock3'
    drop_table(conn, tablename)
    exec_stmt(conn, 'CREATE TABLE %s (a int, b int) engine=columnstore' % tablename)
   
    d = pymcsapi.ColumnStoreDriver()

    try:
        # verify that table is locked
        b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
        assert True == d.isTableLocked(DB_NAME, tablename)
        # verify that table is not locked after rollback
        b.rollback()
        assert False == d.isTableLocked(DB_NAME, tablename)
    except RuntimeError as err:
        pytest.fail("Error executing driver operation: %s" % (err,)) 
 
    try:
        drop_table(conn, tablename)
    except mariadb.Error as err:
        pytest.fail("Error during cleanup: %s" %(err,))
    finally:  
        conn.close()
    
def test_clear_table_lock():
    conn = create_conn()
    tablename = 'pymcsapi_lock4'
    drop_table(conn, tablename)
    exec_stmt(conn, 'CREATE TABLE %s (a int, b int) engine=columnstore' % tablename)
   
    d = pymcsapi.ColumnStoreDriver()

    try:
        lockedTablesAtBeginning = len(d.listTableLocks())
        # initiate ingestion and verify that tablename is locked
        b1 = d.createBulkInsert(DB_NAME, tablename, 0, 0)
        for i in range(0, 100001):
            b1.setColumn(0, 0)
            b1.setColumn(1, 1)
            b1.writeRow()
        assert True == d.isTableLocked(DB_NAME, tablename)
        d.clearTableLock(DB_NAME, tablename)
        # verify that tablename is not locked after clearTableLock
        assert False == d.isTableLocked(DB_NAME, tablename)
        # use bulk 2 to ingest one row
        b2 = d.createBulkInsert(DB_NAME, tablename, 0, 0)
        b2.setColumn(0, 23)
        b2.setColumn(1, 42)
        b2.writeRow()
        lks = d.listTableLocks()
        b2.commit()
        # verify that tablename is not locked after commit of bulk2
        assert False == d.isTableLocked(DB_NAME, tablename)
        
        # verify that an error is thrown if lock does not exist
        oid = d.getSystemCatalog().getTable(DB_NAME, tablename).getOID()
        lockId = -1
        for l in lks:
            if l.tableOID == oid:
                lockId = l.id
        
        assert lockId > -1
        with pytest.raises(RuntimeError) as exceptionInfo:
            d.clearTableLock(lockId)
        assert exceptionInfo.value.args[0] == "Error, no lock found for lockId: %d" % (lockId,)
        
    except RuntimeError as err:
        pytest.fail("Error caught: %s" % (err,)) 
 
    # verify that only one row was written from bulk2 and bulk1's write was rolled back
    try:
        cursor = conn.cursor()
        cursor.execute('select count(*) cnt from %s' % tablename)
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == 1
        cursor.execute('select b from %s WHERE a=23' % tablename)
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == 42
        drop_table(conn, tablename)
    except mariadb.Error as err:
        pytest.fail("Error executing query: %s" %(err,))
    finally:
        if cursor: cursor.close()
        conn.close()
    
