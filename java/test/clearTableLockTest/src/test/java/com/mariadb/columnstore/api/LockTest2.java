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

public class LockTest2 extends Common {

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
