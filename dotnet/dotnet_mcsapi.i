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

%module dotnet_mcsapi

// Get proper PascalCaseIdentifiers
%rename("%(ctitle)s") "";

%include exception.i

// http://www.swig.org/Doc3.0/CSharp.html#CSharp_exceptions
%exception {
  try {
    $action
  } catch (mcsapi::ColumnStoreError &e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, e.what());
    return $null;
  } catch (std::bad_alloc &er) {
    SWIG_exception(SWIG_RuntimeError,const_cast<char*>(er.what()));
  }  
}

%{
#include "libmcsapi/mcsapi.h"
%}

/* MCOL-1321 */
%include "typemaps.i"
%apply int *OUTPUT { mcsapi::columnstore_data_convert_status_t* status };
/* MCOL-1321 */

/* swig includes for standard types / exceptions */
%include <std_except.i>
%include <std_string.i>
%include <stdint.i>

/* include each of the mcsapi.h files and dependencies directly for swig to process */
%include "libmcsapi/visibility.h"
%include "libmcsapi/mcsapi_types.h"
%include "libmcsapi/mcsapi_exception.h"
%include "libmcsapi/mcsapi_driver.h"
%include "libmcsapi/mcsapi_bulk.h"

