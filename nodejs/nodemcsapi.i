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

%module nodemcsapi

/* rename overloaded functions of datatype Number to be accessible and not default to uint64 */
%rename("setColumn_uint64") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, uint64_t);
%rename("setColumn_int64") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, int64_t);
%rename("setColumn_uint32") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, uint32_t);
%rename("setColumn_int32") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, int32_t);
%rename("setColumn_uint16") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, uint16_t);
%rename("setColumn_int16") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, int16_t);
%rename("setColumn_uint8") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, uint8_t);
%rename("setColumn_int8") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, int8_t);
%rename("setColumn_double") mcsapi::ColumnStoreBulkInsert::setColumn(uint16_t, double);

 
%{
#include "libmcsapi/mcsapi.h"
%}

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
