#!/bin/python

# Copyright (c) 2017, MariaDB Corporation. All rights reserved.
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
import pymcsapi

DB_NAME = 'mcsapi'

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
    
    
def test_million_row():
    conn = create_conn()
    tablename = 'pymcsapi_millionrow'
    drop_table(conn, tablename)
    exec_stmt(conn, 'CREATE TABLE %s (a int, b int) engine=columnstore' % tablename)
   
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    rows = 1000000
    try:
        for i in xrange(0, rows):
            b.setColumn(0, i)
            b.setColumn(1, rows - i)
            b.writeRow()
        b.commit()
    except RuntimeError as err:
        b.rollback()
        pytest.fail("Error executing bulk insert: %s" % (err,)) 
 
    try:
        cursor = conn.cursor()
        cursor.execute('select count(*) cnt from %s' % tablename)
        row = cursor.fetchone()
        assert row is not None
        assert row[0] == rows
    except mariadb.Error as err:
        pytest.fail("Error executing query: %s, error: %s" %(query_all_types,err))
    finally:
        if cursor: cursor.close()
        
    drop_table(conn, tablename)
    conn.close()
    

    