package com.mariadb.columnstore.api;

import java.sql.*;
import static org.junit.Assert.*;

public abstract class Common {
    protected static final String DB_NAME = "mcsapi";

    // load java mcsapi native library.
    static {
        System.loadLibrary("javamcsapi");
    }

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
        try {
            conn = DriverManager.getConnection("jdbc:mariadb://localhost/", "root", null);
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
