package com.mariadb.columnstore.api;

/*
 Copyright (c) 2017, MariaDB Corporation. All rights reserved.

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

public class BasicTest extends Common {

    @Test public void testBasic() {
        // create test table
        Connection conn = getConnection();
        String TABLE_NAME = "i1";
        executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(i int, c char(3)) ENGINE=columnstore");

        // simple 1 row test
        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        try {
            b.setColumn(0, 1);
            b.setColumn(1, "jkl");
            b.writeRow();
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
            rs = stmt.executeQuery("SELECT i,c FROM " + TABLE_NAME);
            assertTrue(rs.next());
            assertEquals(1,rs.getInt(1));
            assertEquals("jkl",rs.getString(2));
        }
        catch (SQLException e) {
            fail("Error while validating results: " + e);
        }
        finally {
            close(rs);
            close(stmt);
        }

        // do some catalog checks
        ColumnStoreSystemCatalog catalog = d.getSystemCatalog();
        ColumnStoreSystemCatalogTable cTable = catalog.getTable(DB_NAME, TABLE_NAME);
        assertNotNull(cTable);
        assertEquals(DB_NAME, cTable.getSchemaName());
        assertEquals(TABLE_NAME, cTable.getTableName());
        assertEquals(2, cTable.getColumnCount());
        assert(cTable.getOID() >= 3000);
        ColumnStoreSystemCatalogColumn cCol1 = cTable.getColumn(0);
        assertEquals("i", cCol1.getColumnName());
        assertEquals(columnstore_data_types_t.DATA_TYPE_INT, cCol1.getType());
        assertTrue(cCol1.isNullable());
        ColumnStoreSystemCatalogColumn cCol2 = cTable.getColumn(1);
        assertEquals("c", cCol2.getColumnName());
        assertEquals(columnstore_data_types_t.DATA_TYPE_CHAR, cCol2.getType());
        assertEquals(3, cCol2.getWidth());
        assertTrue(cCol2.isNullable());
        // drop test table
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        close(conn);
    }

    private void verifyAllTypes(Connection conn, long id, String expected) {
        String QUERY_ALL_TYPES = "select uint64, int64, uint32, int32, uint16, int16, uint8, `int8`, " +
                "f, d, ch4, vch30, dt, dtm, ti, ti6, dc, tx from all_types where uint64 = ?";
        PreparedStatement stmt = null;
        ResultSet rs = null;
        StringBuffer str = new StringBuffer();
        try {
           stmt = conn.prepareStatement(QUERY_ALL_TYPES);
           stmt.setLong(1, id);
           rs = stmt.executeQuery();
           assertTrue(rs.next());
           int colCount = stmt.getMetaData().getColumnCount();
           for (int i=1; i<=colCount; i++) {
               if (i>1) str.append(", ");
               str.append(rs.getObject(i));
           }
           assertEquals(expected, str.toString());
        }
        catch (SQLException e) {
            fail("Error while validating all_types results for id: " + id + ", error:" + e);
        } catch (org.junit.ComparisonFailure f){
            fail(f + "\n\nInjected data doesn't match expected data.\nexpected: " + expected + "\nactual:   " + str.toString());
        }
        finally {
            close(rs);
            close(stmt);
        }
    }

    @Test public void testAllTypes() {
        String TABLE_NAME = "all_types";
        String CREATE_ALL_TYPES = "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(" +
                "uint64 bigint unsigned, " +
                "int64 bigint, " +
                "uint32 int unsigned, " +
                "int32 int," +
                "uint16 smallint unsigned, " +
                "int16 smallint, " +
                "uint8 tinyint unsigned, " +
                "`int8` tinyint, " +
                "f float, " +
                "d double, " +
                "ch4 char(5), " +
                "vch30 varchar(30), " +
                "dt date, " +
                "dtm datetime, " +
                "ti time, " +
                "ti6 time(6), " +
                "dc decimal(18), " +
                "tx text " +
                ") engine=columnstore";
        Connection conn = getConnection();
        executeStmt(conn, CREATE_ALL_TYPES);

        ColumnStoreDriver d = new ColumnStoreDriver();
        ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, TABLE_NAME, (short)0, 0);
        try {
            b.setColumn(0, 1L);
            b.setColumn(1,2L);
            b.setColumn(2, 3);
            b.setColumn(3,4);
            b.setColumn(4,5);
            b.setColumn(5,6);
            b.setColumn(6, 7);
            b.setColumn(7,8);
            b.setColumn(8,1.234F);
            b.setColumn(9, 2.34567F);
            b.setColumn(10, "ABCD");
            b.setColumn(11, "Hello World");
            b.setColumn(12, new ColumnStoreDateTime(2017, 9, 8));
            b.setColumn(13, new ColumnStoreDateTime(2017, 9, 8, 13, 58, 23));
            b.setColumn(14, new ColumnStoreTime());
            b.setColumn(15, new ColumnStoreTime());
            b.setColumn(16, new ColumnStoreDecimal(123));
            b.setColumn(17, "Hello World Longer");
            b.writeRow();

            // min values
            b.setColumn(0, 0L);
            b.setColumn(1,-9223372036854775806L);
            b.setColumn(2,0);
            b.setColumn(3,-2147483646);
            b.setColumn(4,0);
            b.setColumn(5,-32766);
            b.setColumn(6,0);
            b.setColumn(7,-126);
            b.setColumn(8, 1.234F);
            b.setColumn(9, 2.34567F);
            b.setColumn(10, "A");
            b.setColumn(11, "B");
            b.setColumn(12, new ColumnStoreDateTime(1000, 1, 1));
            b.setColumn(13, new ColumnStoreDateTime(1000, 1, 1, 0, 0, 0));
            b.setColumn(14, new ColumnStoreTime(-23,59,59));
            b.setColumn(15, new ColumnStoreTime(-23,59,59,999999));
            b.setColumn(16, new ColumnStoreDecimal(-123));
            b.setColumn(17, "C");
            b.writeRow();

            // max values
            b.setColumn(0, 9223372036854775807L);
            b.setColumn(1,9223372036854775807L);
            b.setColumn(2,4294967293L);
            b.setColumn(3, 2147483647);
            b.setColumn(4, 65533);
            b.setColumn(5,32767);
            b.setColumn(6,253);
            b.setColumn(7,127);
            b.setColumn(8,1.234F);
            b.setColumn(9, 2.34567F);
            b.setColumn(10, "ZYXW");
            b.setColumn(11, "012345678901234567890123456789");
            b.setColumn(12, new ColumnStoreDateTime(9999, 12, 31));
            b.setColumn(13, new ColumnStoreDateTime(9999, 12, 31, 23, 59, 59));
            b.setColumn(14, new ColumnStoreTime(23,59,59));
            b.setColumn(15, new ColumnStoreTime(23,59,59,999999));
            b.setColumn(16, new ColumnStoreDecimal(123));
            b.setColumn(17, "012345678901234567890123456789");
            b.writeRow();

            b.commit();
        } catch (Exception e) {
            b.rollback();
            fail("Error during mcsapi write operations: " + e);
        }

        // verify data
        verifyAllTypes(conn, 1L, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.345669984817505, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23.0, 00:00:00, 00:00:00, 123, Hello World Longer");
        verifyAllTypes(conn, 0L, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.345669984817505, A, B, 1000-01-01, 1000-01-01 00:00:00.0, 01:59:59, 01:59:59, -123, C");
        verifyAllTypes(conn, 9223372036854775807L, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.345669984817505, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59.0, 23:59:59, 23:59:59, 123, 012345678901234567890123456789");

        // drop test table
        executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
        close(conn);
    }
}
