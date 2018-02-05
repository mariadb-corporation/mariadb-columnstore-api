package com.mariadb.columnstore.api;

public class ColumnStoreException extends java.lang.RuntimeException{
    public ColumnStoreException() { super(); }
    public ColumnStoreException(String message) { super(message); }
    public ColumnStoreException(String message, Throwable cause) { super(message, cause); }
    public ColumnStoreException(Throwable cause) { super(cause); }
}
