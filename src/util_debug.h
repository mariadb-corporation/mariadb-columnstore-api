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

#pragma once

#include <stdarg.h>

void mcsdebug_set(uint8_t level);

uint8_t mcsdebug_get();

void mcsdebug_impl(const char* MSG, const char* file, size_t line, va_list argptr);

inline void mcsdebug(const char* MSG, ...)
{
    va_list argptr;
    va_start(argptr, MSG);
#ifdef _WIN32
    mcsdebug_impl(MSG, __FILE__, __LINE__, argptr);
#endif
#ifdef __linux__
    mcsdebug_impl(MSG, __FILENAME__, __LINE__, argptr);
#endif
    va_end(argptr);
}

void mcsdebug_hex_impl(const char* DATA, size_t LEN, const char* file, size_t line);

inline void mcsdebug_hex(const char* DATA, size_t LEN)
{
#ifdef _WIN32
    mcsdebug_hex_impl(DATA, LEN, __FILE__, __LINE__);
#endif
#ifdef __linux__
    mcsdebug_hex_impl(DATA, LEN, __FILENAME__, __LINE__);
#endif
}
