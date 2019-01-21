/* Copyright (c) 2019, MariaDB Corporation. All rights reserved.
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
        ColumnStoreBulkInsert b = new ColumnStoreBulkInsert(cPtr, true);
        b.addReference(this);
        return b;
      }
  }

%typemap(javaimports) mcsapi::ColumnStoreDriver %{
  //MCOL-1521 imports begin
  import java.util.Enumeration;
  import java.net.URL;
  import java.util.jar.Manifest;
  import java.io.IOException;
  //MCOL-1521 imports end
%}

%typemap(javacode) mcsapi::ColumnStoreDriver %{
  //MCOL-1521 add a function to display the javamcsapi version which not necessarily needs to be the same than the one of mcsapi
  public String getJavaMcsapiVersion() {
    try{
      Enumeration<URL> resources = getClass().getClassLoader().getResources("META-INF/MANIFEST.MF");
    
      while(resources.hasMoreElements()){
        Manifest manifest = new Manifest(resources.nextElement().openStream());
        if (manifest.getMainAttributes().getValue("name") != null && manifest.getMainAttributes().getValue("name").equals("mcsapi")){
          String version = manifest.getMainAttributes().getValue("build-version");
          String revision = manifest.getMainAttributes().getValue("build-revision");
          String rtn = "";
          if (version != null){
            rtn += version + "-";
          }
          if (revision != null){
            rtn += revision;
          }
          if (! rtn.equals("")){
            return rtn;
          }
        }
      }
    } catch(IOException e){
        //DO NOTHING
    }
    return "unknown";
  }
%}

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

/* simplyfing enums without initializer */
%include "enums.swg"

%typemap(javain) enum SWIGTYPE "$javainput.ordinal()"
%typemap(javaout) enum SWIGTYPE {
    return $javaclassname.class.getEnumConstants()[$jnicall];
  }
%typemap(javabody) enum SWIGTYPE ""

/* MCOL-1321 */
%include "typemaps.i"
%apply int *OUTPUT { mcsapi::columnstore_data_convert_status_t* status };
 
/* swig includes for standard types / exceptions */
%include <std_except.i>
%include <stdint.i>
%include <std_string.i>

%{
#include "libmcsapi/mcsapi.h"
%}

/* MCOL-1094 && MCOL-1961*/
%include "std_vector.i"
%template(TableLockInfoVector) std::vector<mcsapi::TableLockInfo>;
%template(dbRootListVector) std::vector<std::uint32_t>;
typedef long long time_t;

/* include each of the mcsapi.h files and dependencies directly for swig to process */
%include "libmcsapi/visibility.h"
%include "libmcsapi/mcsapi_types.h"
%include "libmcsapi/mcsapi_exception.h"
%include "libmcsapi/mcsapi_driver.h"
%include "libmcsapi/mcsapi_bulk.h"

/* let the parent class load the system library to avoid exceptions if it is otherwise loaded by a child class, i.e. in jupyter scala notebooks*/
%pragma(java) jniclasscode=%{
  static {
    //On Windows try to load javamcsapi.dll's dependent libraries libiconv.dll, libxml2.dll, libuv.dll and mcsapi.dll from java_library_path as Windows only loads them from %PATH% and the directory of the executable.
    if(System.getProperty("os.name").startsWith("Windows")){
        try{ System.loadLibrary("libiconv"); } catch(UnsatisfiedLinkError e){}
        try{ System.loadLibrary("libxml2"); } catch(UnsatisfiedLinkError e){}
        try{ System.loadLibrary("libuv"); } catch(UnsatisfiedLinkError e){}
        try{ System.loadLibrary("mcsapi"); } catch(UnsatisfiedLinkError e){}
    }
    try {
      System.loadLibrary("javamcsapi");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load by parent classloader. \nEnsure that it is loaded by a child classloader\n" + e);
    }
  }
%}
