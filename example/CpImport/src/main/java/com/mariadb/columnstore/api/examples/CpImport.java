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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

import com.mariadb.columnstore.api.ColumnStoreBulkInsert;
import com.mariadb.columnstore.api.ColumnStoreDriver;
import com.mariadb.columnstore.api.ColumnStoreSummary;
import com.mariadb.columnstore.api.ColumnStoreSystemCatalog;
import com.mariadb.columnstore.api.ColumnStoreSystemCatalogTable;
import com.mariadb.columnstore.api.columnstore_data_types_t;

public class CpImport {

	/**
	 * Command line interpreter to import one csv-encoded file with delimiter | to
	 * the database.
	 * 
	 * @param args,
	 *            db-name, table-name, import-file, [dateFormat, dateTimeFormat]
	 */
	public static void main(String[] args) {

		// Check for parameter integrity
		if (args.length < 3) {
			System.err.println("required arguments: db-name, table-name, import-file, [dateFormat, dateTimeFormat]");
			System.err.println(
					"If using an ambigous date format, use the Java SimpleDateFormat notation for dateFormat and dateTimeFormat.");
			System.exit(2);
		}

		// Check if importFile exists
		File sourceFile = new File(args[2]);
		if (!sourceFile.exists()) {
			System.err.println("import-file doesn't exist");
			System.exit(2);
		}

		// Import without date parsing
		if (args.length == 3) {
			CpImport cpImport = new CpImport(args[0], "\\|");
			cpImport.importFile(args[1], sourceFile);
			System.exit(0);
		}

		// Execute with ambiguous date format
		if (args.length == 4) {
			CpImport cpImport = new CpImport(args[0], "\\|");
			cpImport.importFile(args[1], sourceFile, args[3], args[3]);
			System.exit(0);
		}

		// Execute with ambiguous date format
		if (args.length == 5) {
			CpImport cpImport = new CpImport(args[0], "\\|");
			cpImport.importFile(args[1], sourceFile, args[3], args[4]);
			System.exit(0);
		}
	}

	// load java mcsapi native library.
	static {
		System.loadLibrary("_javamcsapi");
	}

	private ColumnStoreDriver d;
	private final String DB_NAME;
	private String delimiter;

	/**
	 * Constructor to create a new CpImport object and establish a database
	 * connection.
	 * 
	 * @param dbName,
	 *            target database
	 * @param delimiter,
	 *            used in the import-csv-file
	 */
	public CpImport(String dbName, String delimiter) {
		d = new ColumnStoreDriver();
		this.DB_NAME = dbName;
		this.delimiter = delimiter;
	}

	/**
	 * Imports the csv-encoded file.
	 * 
	 * @param targetTable,
	 *            target table
	 * @param importFile,
	 *            csv-encoded file to import
	 */
	public void importFile(String targetTable, File importFile) {
		ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, targetTable, (short) 0, 0);

		// get the row count of targetTable
		ColumnStoreSystemCatalog catalog = d.getSystemCatalog();
		ColumnStoreSystemCatalogTable table = catalog.getTable(DB_NAME, targetTable);
		int targetTableColumnCount = table.getColumnCount();

		// insert row by row into targetTable from importFile
		try (BufferedReader br = new BufferedReader(new FileReader(importFile))) {
			String line;
			while ((line = br.readLine()) != null) {
				String[] columns = line.split(delimiter);
				for (int i = 0; i < columns.length; i++) {
					if (i < targetTableColumnCount) {
						b.setColumn(i, columns[i]);
					}
				}
				b.writeRow();
			}
			b.commit();
		} catch (FileNotFoundException e) {
			System.err.println("import-file doesn't exist");
			System.exit(3);
		} catch (IOException e) {
			System.err.println("error while reading / parsing the input-file: " + e.getMessage());
		} catch (Exception e) {
			System.err.println("error during mcsapi write operations: " + e.getMessage());
		}

		// print a short summary of the insertion process
		ColumnStoreSummary summary = b.getSummary();
		System.out.println("Execution time: " + summary.getExecutionTime());
		System.out.println("Rows inserted: " + summary.getRowsInsertedCount());
		System.out.println("Truncation count: " + summary.getTruncationCount());
		System.out.println("Saturated count: " + summary.getSaturatedCount());
		System.out.println("Invalid count: " + summary.getInvalidCount());
		return;
	}

	/**
	 * Imports the csv-encoded file and parses dates according to dateFormat and
	 * dateTimeFormat.
	 * 
	 * @param targetTable,
	 *            target table
	 * @param importFile,
	 *            csv-encoded file to import
	 * @param dateFormat,
	 *            date format to interpret based on Java's SimpleDateFormat notation
	 * @param dateTimeFormat,
	 *            dateTime format to interpret based on Java's SimpleDateFormat
	 *            notation
	 */
	public void importFile(String targetTable, File importFile, String dateFormat, String dateTimeFormat) {
		ColumnStoreBulkInsert b = d.createBulkInsert(DB_NAME, targetTable, (short) 0, 0);

		// get the row count of targetTable
		ColumnStoreSystemCatalog catalog = d.getSystemCatalog();
		ColumnStoreSystemCatalogTable table = catalog.getTable(DB_NAME, targetTable);
		int targetTableColumnCount = table.getColumnCount();

		// insert row by row into targetTable from importFile
		try (BufferedReader br = new BufferedReader(new FileReader(importFile))) {
			String line;
			while ((line = br.readLine()) != null) {
				String[] columns = line.split(delimiter);
				for (int i = 0; i < columns.length; i++) {
					if (i < targetTableColumnCount) {
						if (table.getColumn(i).getType().equals(columnstore_data_types_t.DATA_TYPE_DATE)) {
							b.setColumn(i, parseDate(columns[i], dateFormat));
						} else if (table.getColumn(i).getType().equals(columnstore_data_types_t.DATA_TYPE_DATETIME)) {
							b.setColumn(i, parseDateTime(columns[i], dateTimeFormat));
						} else {
							b.setColumn(i, columns[i]);
						}
					}
				}
				b.writeRow();
			}
			b.commit();
		} catch (FileNotFoundException e) {
			System.err.println("import-file doesn't exist");
			System.exit(3);
		} catch (IOException e) {
			System.err.println("error while reading / parsing the input-file: " + e.getMessage());
		} catch (Exception e) {
			System.err.println("error during mcsapi write operations: " + e.getMessage());
		}

		// print a short summary of the insertion process
		ColumnStoreSummary summary = b.getSummary();
		System.out.println("Execution time: " + summary.getExecutionTime());
		System.out.println("Rows inserted: " + summary.getRowsInsertedCount());
		System.out.println("Truncation count: " + summary.getTruncationCount());
		System.out.println("Saturated count: " + summary.getSaturatedCount());
		System.out.println("Invalid count: " + summary.getInvalidCount());
		return;
	}

	/**
	 * Parses the date to a MariaDB supported format.
	 * 
	 * @param date,
	 *            input to import
	 * @param dateFormat,
	 *            of input in SimpleDateFormat notation
	 * @return parsed date in MariaDB format, or null in case of error.
	 */
	private String parseDate(String date, String dateFormat) {
		SimpleDateFormat inputParser = new SimpleDateFormat(dateFormat);
		Date d = null;
		try {
			d = inputParser.parse(date);
		} catch (ParseException e) {
			System.err.println("error, date '" + date + "' couldn't be parsed.");
			return null;
		}
		SimpleDateFormat outputParser = new SimpleDateFormat("yyyy-MM-dd");

		return outputParser.format(d);
	}

	/**
	 * Parses the dateTime to a MariaDB supported format.
	 * 
	 * @param dateTime,
	 *            input to import
	 * @param dateTimeFormat,
	 *            of input in SimpleDateFormat notation
	 * @return parsed dateTime in MariaDB format, or null in case of error.
	 */
	private String parseDateTime(String date, String dateTimeFormat) {
		SimpleDateFormat inputParser = new SimpleDateFormat(dateTimeFormat);
		Date d = null;
		try {
			d = inputParser.parse(date);
		} catch (ParseException e) {
			System.err.println("error, dateTime '" + date + "' couldn't be parsed.");
			return null;
		}
		SimpleDateFormat outputParser = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

		return outputParser.format(d);
	}
}
