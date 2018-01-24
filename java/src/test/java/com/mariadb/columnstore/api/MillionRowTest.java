package com.mariadb.columnstore.api;

import org.junit.Test;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import static org.junit.Assert.*;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class MillionRowTest extends Common {

    @Test public void testLoadMillionRows() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME = "jmcsapi_million_row";
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(a int, b int) engine=columnstore");

        // simple 1 row test
        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        int rows = 1000000;
        try {
            for (int i=0; i<rows; ++i) {
                b.setColumn(0, i);
                b.setColumn(1, rows-i);
                b.writeRow();
            }
            b.commit();                            
        } catch (Exception e) {
            b.rollback();
            fail("Error during mcsapi write operations: " + e);
        }

        // verify results
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
}
