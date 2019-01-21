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

%exception {
  try {
    $action
  } catch (mcsapi::ColumnStoreError &e) {
    PyErr_SetString(PyExc_RuntimeError, const_cast<char*>(e.what()));
    SWIG_fail;
  } catch (std::bad_alloc &er) {
    PyErr_SetString(PyExc_RuntimeError, const_cast<char*>(er.what()));
    SWIG_fail;
  }
}

%module pymcsapi

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

/* MCOL-1094 && MCOL-1961 begin */
%include "std_vector.i"
%template(TableLockInfoVector) std::vector<mcsapi::TableLockInfo>;
%template(dbRootListVector) std::vector<std::uint32_t>;
%typemap(in) time_t
{
    if (PyLong_Check($input))
        $1 = (time_t) PyLong_AsLong($input);
    else if (PyInt_Check($input))
        $1 = (time_t) PyInt_AsLong($input);
    else if (PyFloat_Check($input))
        $1 = (time_t) PyFloat_AsDouble($input);
    else {
        PyErr_SetString(PyExc_TypeError,"Expected a number for time_t");
        return NULL;
    }
}

%typemap(out) time_t
{
    $result = PyLong_FromLong((long)$1);
}
/* MCOL-1094 && MCOL-1961 end */

/* include each of the mcsapi.h files and dependencies directly for swig to process */
%include "libmcsapi/visibility.h"
%include "libmcsapi/mcsapi_types.h"
%include "libmcsapi/mcsapi_exception.h"
%include "libmcsapi/mcsapi_driver.h"
%include "libmcsapi/mcsapi_bulk.h"
