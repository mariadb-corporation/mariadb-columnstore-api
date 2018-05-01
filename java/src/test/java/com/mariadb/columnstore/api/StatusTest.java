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

import org.junit.Test;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import static org.junit.Assert.*;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class StatusTest extends Common {

    /**
    / Test that row ingestion works
    */
    @Test public void testInserted() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME = "java_summary_ins";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b varchar(10)) engine=columnstore");
        close(conn);

        // simple 1000 row ingestion test
        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        columnstore_data_convert_status_t status;
        int rows = 1000;
        int[] s = {-1};
        try {
            for (int i=0; i<rows; ++i) {
                b.setColumn(0, i, s);
                status = columnstore_data_convert_status_t.values()[s[0]];
                assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_NONE);
                b.setColumn(1, "ABC", s);
                status = columnstore_data_convert_status_t.values()[s[0]];
                assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_NONE);
                b.writeRow();
            }
            b.commit();
        } catch (Exception e) {
            b.rollback();
            fail("Error during mcsapi write operations: " + e);
        }

        // verify results
        conn = getConnection();
        ColumnStoreSummary summary = b.getSummary();
        assertEquals(summary.getRowsInsertedCount().intValue(), rows); 
        
        Statement stmt = null;
        ResultSet rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery("SELECT count(*) cnt FROM " + TABLE_NAME);
            assertTrue(rs.next());
            assertEquals(rows,rs.getInt(1));
        }
        catch (SQLException e) {
            fail("Error while validating results: " + e);
        }
        finally {
            close(rs);
            close(stmt);
        }

        // drop test table
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        close(conn);
    }
    
    /**
    / Test that saturated works
    */
    @Test public void testSaturated() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME = "java_summary_sat";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b varchar(10)) engine=columnstore");
        close(conn);

        // simple row ingestion test
        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        columnstore_data_convert_status_t status;
        int[] s = {-1};
        try {
            b.setColumn(0, Long.MAX_VALUE, s);
            status = columnstore_data_convert_status_t.values()[s[0]];
            assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_SATURATED);
            b.setColumn(1, "ABC", s);
            status = columnstore_data_convert_status_t.values()[s[0]];
            assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_NONE);
            b.writeRow();
            b.rollback();
        } catch (Exception e) {
            fail("Error during mcsapi write operations: " + e);
        }

        // verify results
        conn = getConnection();
        ColumnStoreSummary summary = b.getSummary();
        assertEquals(summary.getSaturatedCount().intValue(), 1); 
        
        Statement stmt = null;
        ResultSet rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery("SELECT count(*) cnt FROM " + TABLE_NAME);
            assertTrue(rs.next());
            assertEquals(0,rs.getInt(1));
        }
        catch (SQLException e) {
            fail("Error while validating results: " + e);
        }
        finally {
            close(rs);
            close(stmt);
        }

        // drop test table
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        close(conn);
    }
    
    /**
    / Test that invalid works
    */
    @Test public void testInvalid() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME = "java_summary_inv";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b varchar(10)) engine=columnstore");
        close(conn);

        // simple row ingestion test
        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        columnstore_data_convert_status_t status;
        int[] s = {-1};
        try {
            b.setColumn(0, "abc", s);
            status = columnstore_data_convert_status_t.values()[s[0]];
            assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_INVALID);
            b.setColumn(1, "ABC", s);
            status = columnstore_data_convert_status_t.values()[s[0]];
            assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_NONE);
            b.writeRow();
            b.rollback();
        } catch (Exception e) {
            fail("Error during mcsapi write operations: " + e);
        }

        // verify results
        conn = getConnection();
        ColumnStoreSummary summary = b.getSummary();
        assertEquals(summary.getInvalidCount().intValue(), 1); 
        
        Statement stmt = null;
        ResultSet rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery("SELECT count(*) cnt FROM " + TABLE_NAME);
            assertTrue(rs.next());
            assertEquals(0,rs.getInt(1));
        }
        catch (SQLException e) {
            fail("Error while validating results: " + e);
        }
        finally {
            close(rs);
            close(stmt);
        }

        // drop test table
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        close(conn);
    }
    
    /**
    / Test that truncated works
    */
    @Test public void testTruncated() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME = "java_summary_trun";
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b varchar(10)) engine=columnstore");
        close(conn);

        // simple row ingestion test
        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        columnstore_data_convert_status_t status;
        int[] s = {-1};
        try {
            b.setColumn(0, 2342, s);
            status = columnstore_data_convert_status_t.values()[s[0]];
            assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_NONE);
            b.setColumn(1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", s);
            status = columnstore_data_convert_status_t.values()[s[0]];
            assertEquals(status, columnstore_data_convert_status_t.CONVERT_STATUS_TRUNCATED);
            b.writeRow();
            b.commit();
        } catch (Exception e) {
            b.rollback();
            fail("Error during mcsapi write operations: " + e);
        }

        // verify results
        conn = getConnection();
        ColumnStoreSummary summary = b.getSummary();
        assertEquals(summary.getTruncationCount().intValue(), 1); 
        
        Statement stmt = null;
        ResultSet rs = null;
        try {
            stmt = conn.createStatement();
            rs = stmt.executeQuery("SELECT count(*) cnt FROM " + TABLE_NAME);
            assertTrue(rs.next());
            assertEquals(1,rs.getInt(1));
        }
        catch (SQLException e) {
            fail("Error while validating results: " + e);
        }
        finally {
            close(rs);
            close(stmt);
        }

        // drop test table
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        close(conn);
    }
}
