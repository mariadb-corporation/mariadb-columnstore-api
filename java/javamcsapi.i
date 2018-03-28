/* Copyright (c) 2017, MariaDB Corporation. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 */

%typemap(javacode) mcsapi::ColumnStoreSystemCatalog %{
  // MCOL-1091: Ensure that the GC doesn't collect any ColumnStoreDriver instance set from Java
  private ColumnStoreDriver columnStoreDriverReference;
  protected void addReference(ColumnStoreDriver columnStoreDriver){
    columnStoreDriverReference = columnStoreDriver;
  }
%}

%typemap(javacode) mcsapi::ColumnStoreBulkInsert %{
  // MCOL-1091: Ensure that the GC doesn't collect any ColumnStoreDriver instance set from Java
  private ColumnStoreDriver columnStoreDriverReference;
  protected void addReference(ColumnStoreDriver columnStoreDriver){
    columnStoreDriverReference = columnStoreDriver;
  }
%}

%typemap(javaout) mcsapi::ColumnStoreSystemCatalog &mcsapi::ColumnStoreDriver::getSystemCatalog {
    // MCOL-1091: Add a Java reference to prevent premature garbage collection
    $javaclassname c = new $javaclassname($jnicall, false);
    c.addReference(this);
    return c;
  }

%typemap(javaout) mcsapi::ColumnStoreBulkInsert *mcsapi::ColumnStoreDriver::createBulkInsert {
      // MCOL-1091: Add a Java reference to prevent premature garbage collection
      long cPtr = $jnicall;
      if (cPtr == 0){
        return null;
      } else {
        ColumnStoreBulkInsert b = new ColumnStoreBulkInsert(cPtr, false);
        b.addReference(this);
        return b;
      }
  }

%javaexception("com.mariadb.columnstore.api.ColumnStoreException") {
  try {
    $action
  }
  catch ( mcsapi::ColumnStoreError & e ) {
    jclass eclass = jenv->FindClass("com/mariadb/columnstore/api/ColumnStoreException");
    if ( eclass ) {
      jenv->ThrowNew( eclass, e.what() );
    }
  }
  catch ( std::bad_alloc & er ) {
    jclass eclass = jenv->FindClass("com/mariadb/columnstore/api/ColumnStoreException");
    if ( eclass ) {
      jenv->ThrowNew( eclass, er.what() );
    }
  }
}

%module javamcsapi
 
/* swig includes for standard types / exceptions */
%include <std_except.i>
%include <stdint.i>
%include <std_string.i>

%{
#include "libmcsapi/mcsapi.h"
%}

/* include each of the mcsapi.h files and dependencies directly for swig to process */
%include "libmcsapi/visibility.h"
%include "libmcsapi/mcsapi_types.h"
%include "libmcsapi/mcsapi_exception.h"
%include "libmcsapi/mcsapi_driver.h"
%include "libmcsapi/mcsapi_bulk.h"

/* let the parent class load the system library to avoid exceptions if it is otherwise loaded by a child class, i.e. in jupyter scala notebooks*/
%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("javamcsapi");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load by parent classloader. \nEnsure that it is loaded by a child classloader\n" + e);
    }
  }
%}
