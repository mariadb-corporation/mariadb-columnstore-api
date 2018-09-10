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

// Advanced insert example from docs, requires the table in
// advanced_bulk_insert.sql to be created in the test database
//
// NOTE: if you edit this file please update the line numbers in
// advanced_bulk_insert.rst

import com.mariadb.columnstore.api.*;

public class Advanced_bulk_insert {

    public static void main(String[] args) {
        try {
            ColumnStoreDriver d = new ColumnStoreDriver();
            ColumnStoreBulkInsert b = d.createBulkInsert("test", "t2", (short)0, 0);
            b.setColumn(0, 1);
            b.setColumn(1, "Andrew");
            b.setColumn(2, "1936-12-24");
            b.setColumn(3, "2017-07-07 15:14:12");
            b.setColumn(4, "15239.45");
            b.writeRow();
            b.setColumn(0, 2).setColumn(1, "David").setColumn(2, new ColumnStoreDateTime("1972-05-23", "%Y-%m-%d")).setColumn(3, new ColumnStoreDateTime("2017-07-07 15:20:18", "%Y-%m-%d %H:%M:%S")).setColumn(4, new ColumnStoreDecimal(2347623, (short)2)).writeRow();
            b.commit();
            ColumnStoreSummary summary = b.getSummary();
            System.out.println("Execution time: " + summary.getExecutionTime());
            System.out.println("Rows inserted: " + summary.getRowsInsertedCount());
            System.out.println("Truncation count: " + summary.getTruncationCount());
            System.out.println("Saturated count: " + summary.getSaturatedCount());
            System.out.println("Invalid count: " + summary.getInvalidCount());
        }
        catch (ColumnStoreException e) {
            System.err.println("Error caught: " + e.getMessage());
        }
    }
}
