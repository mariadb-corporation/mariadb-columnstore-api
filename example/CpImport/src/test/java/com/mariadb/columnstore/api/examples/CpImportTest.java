package com.mariadb.columnstore.api.examples;

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

import java.io.File;
import java.sql.*;
import org.junit.Test;

import static org.junit.Assert.*;

public class CpImportTest {

	private static final String DB_NAME = "mcsapi";

	private void close(Connection conn) {
		if (conn != null) {
			try {
				conn.close();
			} catch (SQLException e) {
				System.out.println("error closing connection " + e);
			}
		}
	}

	private void close(Statement stmt) {
		if (stmt != null) {
			try {
				stmt.close();
			} catch (SQLException e) {
				System.out.println("error closing statement " + e);
			}
		}
	}

	private void close(ResultSet rs) {
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
	 * 
	 * @return Connection
	 */
	private Connection getConnection() {
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

	private void executeStmt(Connection conn, String sql) {
		Statement stmt = null;
		try {
			stmt = conn.createStatement();
			stmt.execute(sql);
		} catch (SQLException e) {
			fail("Could not execute statement: " + sql + ", error: " + e);
		} finally {
			close(stmt);
		}
	}

	/**
	 * Test without interpretation of date and dateTime fields
	 */
	@Test
	public void testBasicWithoutParsing() {
		// create test table
		Connection conn = getConnection();
		String TABLE_NAME = "t1";
		String DELIMITER = "\\|";
		executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME
				+ "(i int, vc varchar(8), d date, dt datetime) ENGINE=columnstore");

		// simple insert test without parsing
		CpImport imp = new CpImport(DB_NAME, DELIMITER);
		imp.importFile(TABLE_NAME,
				new File(ClassLoader.getSystemClassLoader().getResource("withoutParsing.csv").getFile()));

		// verify results
		Statement stmt = null;
		ResultSet rs = null;
		try {
			stmt = conn.createStatement();
			rs = stmt.executeQuery("SELECT i, vc, d, dt FROM " + TABLE_NAME);
			assertTrue(rs.next());
			assertEquals(88, rs.getInt(1));
			assertEquals("hello123", rs.getString(2));
			assertEquals("2017-12-24", rs.getString(3));
			assertEquals("2088-11-02 12:23:42.0", rs.getString(4));
			assertTrue(rs.next());
			assertEquals(12, rs.getInt(1));
			assertEquals("MariaDB", rs.getString(2));
			assertEquals("1971-01-12", rs.getString(3));
			assertEquals("1988-09-25 01:32:21.0", rs.getString(4));
			assertTrue(rs.next());
			assertEquals(33, rs.getInt(1));
			assertEquals("advice", rs.getString(2));
			assertEquals("2043-04-01", rs.getString(3));
			assertEquals("2017-12-13 16:23:09.0", rs.getString(4));
			assertTrue(rs.next());
			assertEquals(42, rs.getInt(1));
			assertEquals("empty", rs.getString(2));
			assertEquals("0000-00-00", rs.getString(3));
			assertEquals("0000-00-00 00:00:00", rs.getString(4));
		} catch (SQLException e) {
			fail("Error while validating results: " + e);
		} finally {
			close(rs);
			close(stmt);
		}

		// drop test table
		executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
		close(conn);
	}

	/**
	 * Test with interpretation of date and dateTime fields.
	 */
	@Test
	public void testBasicWithParsing() {
		// create test table
		Connection conn = getConnection();
		String TABLE_NAME = "t2";
		String DELIMITER = "\\|";
		executeStmt(conn, "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + "(d date, dt datetime) ENGINE=columnstore");

		// simple insert test without parsing
		CpImport imp = new CpImport(DB_NAME, DELIMITER);
		imp.importFile(TABLE_NAME,
				new File(ClassLoader.getSystemClassLoader().getResource("dateParsing1.csv").getFile()),
				"HH:yy,dd-MMM ss.mm", "HH:yy,dd-MMM ss.mm");
		imp.importFile(TABLE_NAME,
				new File(ClassLoader.getSystemClassLoader().getResource("dateParsing2.csv").getFile()), "MM-dd-yyyy",
				"dd.MM.yyyy HH/mm/ss");

		// verify results
		Statement stmt = null;
		ResultSet rs = null;
		try {
			stmt = conn.createStatement();
			rs = stmt.executeQuery("SELECT d, dt FROM " + TABLE_NAME);
			// dateParsing1.csv
			assertTrue(rs.next());
			assertEquals("1989-07-13", rs.getString(1));
			assertEquals("1989-07-13 15:12:14.0", rs.getString(2));
			assertTrue(rs.next());
			assertEquals("2021-01-12", rs.getString(1));
			assertEquals("2021-01-12 23:42:00.0", rs.getString(2));
			assertTrue(rs.next());
			assertEquals("1973-02-01", rs.getString(1));
			assertEquals("1973-02-01 11:38:12.0", rs.getString(2));
			assertTrue(rs.next());
			assertEquals("2017-12-24", rs.getString(1));
			assertEquals("2017-12-24 18:00:01.0", rs.getString(2));
			// dateParsing2.csv
			assertTrue(rs.next());
			assertEquals("1736-09-14", rs.getString(1));
			assertEquals("1736-09-14 02:03:04.0", rs.getString(2));
			assertTrue(rs.next());
			assertEquals("2006-04-05", rs.getString(1));
			assertEquals("2006-04-05 04:05:59.0", rs.getString(2));
			assertTrue(rs.next());
			assertEquals("2013-07-09", rs.getString(1));
			assertEquals("2013-07-09 13:06:30.0", rs.getString(2));
			assertTrue(rs.next());
			assertEquals("1989-12-02", rs.getString(1));
			assertEquals("1989-12-02 23:42:00.0", rs.getString(2));
		} catch (SQLException e) {
			fail("Error while validating results: " + e);
		} finally {
			close(rs);
			close(stmt);
		}

		// drop test table
		executeStmt(conn, "DROP TABLE IF EXISTS " + TABLE_NAME);
		close(conn);
	}

}
