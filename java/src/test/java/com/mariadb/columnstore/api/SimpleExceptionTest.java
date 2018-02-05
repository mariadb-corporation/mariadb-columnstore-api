package com.mariadb.columnstore.api;

import org.junit.Test;

public class SimpleExceptionTest {

    @Test(expected = ColumnStoreException.class) 
    public void testException() {
	
      ColumnStoreDriver d = new ColumnStoreDriver();
      ColumnStoreSystemCatalog catalog = d.getSystemCatalog();
      ColumnStoreSystemCatalogTable table = catalog.getTable("test", "nonexistent");
    }
}
