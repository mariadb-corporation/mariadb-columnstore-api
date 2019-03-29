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
import static org.junit.Assert.*;

public abstract class Common {
    protected static final String DB_NAME = "mcsapi";

    protected void close(Connection conn) {
        if (conn != null) {
            try {
                conn.close();
            } catch (SQLException e) {
                System.out.println("error closing connection " + e);
            }
        }
    }

    protected void close(Statement stmt) {
        if (stmt != null) {
            try {
                stmt.close();
            } catch (SQLException e) {
                System.out.println("error closing statement " + e);
            }
        }
    }

    protected void close(ResultSet rs) {
        if (rs != null) {
            try {
                rs.close();
            } catch (SQLException e) {
                System.out.println("error closing result set " + e);
            }
        }
    }

    /**
     * Create's a connection and also create the test database if needed
     * @return Connection
     */
    protected Connection getConnection() {
        Connection conn = null;
        Statement stmt = null;
        
        String host = System.getenv("MCSAPI_CS_TEST_IP");
        if (host == null){
            host = "localhost";
        }
        String user = System.getenv("MCSAPI_CS_TEST_USER");
        if (user == null){
            user = "root";
        }
        String password = System.getenv("MCSAPI_CS_TEST_PASSWORD");
        
        try {
            conn = DriverManager.getConnection("jdbc:mariadb://"+host+"/", user, password);
            stmt = conn.createStatement();
            stmt.execute("CREATE DATABASE IF NOT EXISTS " + DB_NAME);
            stmt.execute("USE " + DB_NAME);
            stmt.close();
        } catch (SQLException e) {
            close(stmt);
            close(conn);
            fail("Could not create database, error: " + e);
        }
        return conn;
    }

    protected void executeStmt(Connection conn, String sql) {
        Statement stmt = null;
        try  {
            stmt = conn.createStatement();
            stmt.execute(sql);
        }
        catch (SQLException e) {
            fail("Could not execute statement: " + sql + ", error: " + e);
        }
        finally {
            close(stmt);
        }
    }
}
