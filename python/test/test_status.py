#!/bin/python

# Copyright (c) 2018, MariaDB Corporation. All rights reserved.
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
import pymcsapi, pytest, datetime
import mysql.connector as mariadb
import sys
from six.moves import range

DB_NAME = 'mcsapi'

if sys.version_info[0] == 3:
        long = int

def create_db():
    try:
        conn = mariadb.connect(user='root')
        cursor = conn.cursor();
        cursor.execute("CREATE DATABASE IF NOT EXISTS %s" %(DB_NAME,))
    except mariadb.Error as err:
        pytest.fail("Error creating database %s" %(err,))
    finally:
        if cursor: cursor.close()
        if conn: conn.close()

def create_conn():
    create_db()    
    try:
        return mariadb.connect(user='root', database=DB_NAME)
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

#Test that rows inserted works
def test_inserted():
    conn = create_conn()
    tablename = 'py_summary_ins'
    rows = 1000
    drop_table(conn, tablename)
    exec_stmt(conn, 'create table if not exists %s(a int, b varchar(10)) engine=columnstore' %(tablename,))
    
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    try:
        for i in range(0, rows):
            bulk, status = b.setColumn(0, i)
            assert status == pymcsapi.CONVERT_STATUS_NONE
            bulk, status = b.setColumn(1, 'ABC')
            assert status == pymcsapi.CONVERT_STATUS_NONE
            bulk.writeRow()
        b.commit()
        
        s = b.getSummary()
        assert int(s.getRowsInsertedCount()) == rows
    except RuntimeError as err:
        b.rollback()
        pytest.fail("Error executing status row insertion test: %s" %(err,)) 
        
    try:
        cursor = conn.cursor()
        cursor.execute('select count(*) cnt from %s' % (tablename,))
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == rows
    except mariadb.Error as err:
        pytest.fail("Error executing status row insertion query: %s" %(err,))
    finally:
        if cursor: cursor.close()
    drop_table(conn, tablename)   
    conn.close()

    
#Test that saturated works
def test_saturated():
    conn = create_conn()
    tablename = 'py_summary_sat'
    drop_table(conn, tablename)
    exec_stmt(conn, 'create table if not exists %s(a int, b varchar(10)) engine=columnstore' %(tablename,))
    
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    try:
        bulk, status = b.setColumn(0, 0xFFFFFFFF)
        assert status == pymcsapi.CONVERT_STATUS_SATURATED
        bulk, status = b.setColumn(1, 'ABC')
        assert status == pymcsapi.CONVERT_STATUS_NONE
        bulk.writeRow()
        b.rollback()
        
        s = b.getSummary()
        assert int(s.getSaturatedCount()) == 1
    except RuntimeError as err:
        pytest.fail("Error executing status row saturated test: %s" %(err,)) 
        
    try:
        cursor = conn.cursor()
        cursor.execute('select count(*) cnt from %s' % (tablename,))
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == 0
    except mariadb.Error as err:
        pytest.fail("Error executing status row saturated query: %s" %(err,))
    finally:
        if cursor: cursor.close()
    drop_table(conn, tablename)   
    conn.close()


#Test that invalid works
def test_invalid():
    conn = create_conn()
    tablename = 'py_summary_inv'
    drop_table(conn, tablename)
    exec_stmt(conn, 'create table if not exists %s(a int, b varchar(10)) engine=columnstore' %(tablename,))
    
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    try:
        bulk, status = b.setColumn(0, 'abc')
        assert status == pymcsapi.CONVERT_STATUS_INVALID
        bulk, status = b.setColumn(1, 'ABC')
        assert status == pymcsapi.CONVERT_STATUS_NONE
        bulk.writeRow()
        b.rollback()
        
        s = b.getSummary()
        assert int(s.getInvalidCount()) == 1
    except RuntimeError as err:
        pytest.fail("Error executing status row invalid test: %s" %(err,)) 
        
    try:
        cursor = conn.cursor()
        cursor.execute('select count(*) cnt from %s' % (tablename,))
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == 0
    except mariadb.Error as err:
        pytest.fail("Error executing status row invalid query: %s" %(err,))
    finally:
        if cursor: cursor.close()
    drop_table(conn, tablename)   
    conn.close()
    
    
#Test that saturated works
def test_truncated():
    conn = create_conn()
    tablename = 'py_summary_tru'
    drop_table(conn, tablename)
    exec_stmt(conn, 'create table if not exists %s(a int, b varchar(10)) engine=columnstore' %(tablename,))
    
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    try:
        bulk, status = b.setColumn(0, 23)
        assert status == pymcsapi.CONVERT_STATUS_NONE
        bulk, status = b.setColumn(1, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')
        assert status == pymcsapi.CONVERT_STATUS_TRUNCATED
        bulk.writeRow()
        b.commit()
        
        s = b.getSummary()
        assert int(s.getTruncationCount()) == 1
    except RuntimeError as err:
        pytest.fail("Error executing status row truncation test: %s" %(err,)) 
        
    try:
        cursor = conn.cursor()
        cursor.execute('select count(*) cnt from %s' % (tablename,))
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == 1
    except mariadb.Error as err:
        pytest.fail("Error executing status row truncation query: %s" %(err,))
    finally:
        if cursor: cursor.close()
    drop_table(conn, tablename)   
    conn.close()
