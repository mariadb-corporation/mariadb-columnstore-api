package com.mariadb.columnstore.api;

/*
 Copyright (c) 2018, MariaDB Corporation. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301  USA
*/

import java.sql.*;
import org.junit.Test;
import static org.junit.Assert.*;

public class LockTest extends Common {

    /* Tests if listTableLocks() detects locked tables*/
    @Test public void testListTableLocks() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME1 = "javalock1";
        String TABLE_NAME2 = "javalock2";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME1 );
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME2 );
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME1 + "(a int, b int) ENGINE=columnstore");
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME2 + "(a int, b int) ENGINE=columnstore");
        
        ColumnStoreDriver d = new ColumnStoreDriver();
        try{
            long lockedTablesAtBeginning = d.listTableLocks().size();
            ColumnStoreBulkInsert b1 = d.createBulkInsert(DB_NAME, TABLE_NAME1, (short)0, 0);
            assertEquals((long)1,d.listTableLocks().size()-lockedTablesAtBeginning);
            ColumnStoreBulkInsert b2 = d.createBulkInsert(DB_NAME, TABLE_NAME2, (short)0, 0);
            assertEquals((long)2,d.listTableLocks().size()-lockedTablesAtBeginning);
            b2.rollback();
            assertEquals((long)1,d.listTableLocks().size()-lockedTablesAtBeginning);
            b1.rollback();
            assertEquals((long)0,d.listTableLocks().size()-lockedTablesAtBeginning);
        } catch (Exception e){
            fail("Error during listTableLocks test: " + e);
        }
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME1 );
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME2 );
        close(conn);
    }

    /* Tests if isTableLocked() detects locked tables*/
    @Test public void testIsTableLocked() {
        Connection conn = getConnection();
        String TABLE_NAME = "javalock3";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME );
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b int) ENGINE=columnstore");
        
        ColumnStoreDriver d = new ColumnStoreDriver();
        try{
            ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
            assertTrue(d.isTableLocked(DB_NAME,TABLE_NAME));
            b.rollback();
            assertFalse(d.isTableLocked(DB_NAME,TABLE_NAME));
        } catch (Exception e){
            fail("Error during isTableLocked test: " + e);
        }
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME );
        close(conn);
    }
    
    /* Tests if isTableLocked() and listTableLocks() return the correct TableLockInfo object*/
    @Test public void testListTableLocksIsTableLocked() {
        Connection conn = getConnection();
        String TABLE_NAME = "javalock4";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME );
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b int) ENGINE=columnstore");
        
        ColumnStoreDriver d = new ColumnStoreDriver();
        try{
            ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
            assertTrue(d.isTableLocked(DB_NAME,TABLE_NAME));
            
            // get the first TableLockInfo object from listTableLocks()
            long oid = d.getSystemCatalog().getTable(DB_NAME, TABLE_NAME).getOID();
            TableLockInfoVector tliv = d.listTableLocks();
            TableLockInfo tli1 = null;
            for(long v=0; v<tliv.size(); v++){
                if(tliv.get((int)v).getTableOID() == oid){
                    tli1 = tliv.get((int)v);
                    break;
                }
            }
            assertNotNull(tli1);
            
            // get the second TableLockInfo object from isTableLocked()
            TableLockInfo tli2 = new TableLockInfo();
            assertTrue(d.isTableLocked(DB_NAME,TABLE_NAME,tli2));
            assertNotNull(tli2);
            
            // compare both objects
            assertEquals(tli1.getTableOID(), tli2.getTableOID());
            //assertEquals(tli1.getCreationTime(), tli2.getCreationTime()); //TODO in javamcsapi.i
            assertEquals(tli1.getId(), tli2.getId());
            assertEquals(tli1.getOwnerName(), tli2.getOwnerName());
            assertEquals(tli1.getOwnerPID(), tli2.getOwnerPID());
            assertEquals(tli1.getOwnerSessionID(), tli2.getOwnerSessionID());
            assertEquals(tli1.getOwnerTxnID(), tli2.getOwnerTxnID());
            assertEquals(tli1.getState(), tli2.getState());
            
            // TODO compare the dbrootlist once fixed in javamcsapi.i
            
            // rollback the bulk injection and clear the lock
            b.rollback();
            assertFalse(d.isTableLocked(DB_NAME,TABLE_NAME));
        } catch (Exception e){
            e.printStackTrace();
            fail("Error during ListTableLocksIsTableLocked test: " + e);
        }
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME );
        close(conn);
    }
    
    @Test public void testClearTableLock() {
        Connection conn = getConnection();
        String TABLE_NAME = "javalock5";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME );
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b int) ENGINE=columnstore");
        
        ColumnStoreDriver d = new ColumnStoreDriver();
        try{
            ColumnStoreBulkInsert b1 = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
            for (int i=0; i<=100001; i++){
                b1.setColumn(0, 0);
                b1.setColumn(1, 1);
                b1.writeRow();
            }
            assertTrue(d.isTableLocked(DB_NAME,TABLE_NAME));
            d.clearTableLock(DB_NAME,TABLE_NAME);
            assertFalse(d.isTableLocked(DB_NAME,TABLE_NAME));
            ColumnStoreBulkInsert b2 = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
            b2.setColumn(0, 23);
            b2.setColumn(1, 42);
            b2.writeRow();
            b2.commit();
            assertFalse(d.isTableLocked(DB_NAME,TABLE_NAME));
        } catch (Exception e){
            fail("Error during clearTableLock test: " + e);
        }
        
        // verify that only b2's ingestions were written
        Statement stmt = null;
        ResultSet rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery("SELECT COUNT(*) cnt FROM " + TABLE_NAME);
            assertTrue(rs.next());
            assertEquals(1,rs.getInt(1));
            rs = stmt.executeQuery("SELECT b FROM " + TABLE_NAME + " WHERE a = 23");
            assertTrue(rs.next());
            assertEquals(42,rs.getInt(1));
        }
        catch (SQLException e) {
            fail("Error while validating results: " + e);
        }
        finally {
            close(rs);
            close(stmt);
        }
        
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME );
        close(conn);
    }
}

