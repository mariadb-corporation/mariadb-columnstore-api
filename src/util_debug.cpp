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

#include "common.h"
#include "util_debug.h"

#include <stdarg.h>

static bool debugging_enabled = false;
static bool advanced_debugging_enabled = false;

void mcsdebug_set(bool enabled)
{
    debugging_enabled = enabled;
}

void mcsdebug(const char* MSG, ...)
{
    if (!debugging_enabled)
    {
        return;
    }
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmpdbuf[64], dbuf[64];
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmpdbuf, sizeof tmpdbuf, "%H:%M:%S", nowtm);
    snprintf(dbuf, sizeof dbuf, "%s.%06ld", tmpdbuf, tv.tv_usec);
    va_list argptr;
    va_start(argptr, MSG);
    fprintf(stderr, "[mcsapi][%s] %s:%d ", dbuf,  __FILENAME__, __LINE__);
    vfprintf(stderr, MSG, argptr);
    fprintf(stderr, "\n");
    va_end(argptr);
}


void mcsdebug_advanced_set(bool enabled)
{
    advanced_debugging_enabled = enabled;
}

void mcsdebug_advanced(const char* MSG, ...)
{
    if (!advanced_debugging_enabled)
    {
        return;
    }
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmpdbuf[64], dbuf[64];
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmpdbuf, sizeof tmpdbuf, "%H:%M:%S", nowtm);
    snprintf(dbuf, sizeof dbuf, "%s.%06ld", tmpdbuf, tv.tv_usec);
    va_list argptr;
    va_start(argptr, MSG);
    fprintf(stderr, "[mcsapi][%s] %s:%d ", dbuf,  __FILENAME__, __LINE__);
    vfprintf(stderr, MSG, argptr);
    fprintf(stderr, "\n");
    va_end(argptr);
}


void mcsdebug_hex(const char* DATA, size_t LEN)
{
    if (!debugging_enabled)
    {
        return;
    }
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmpdbuf[64], dbuf[64];
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmpdbuf, sizeof tmpdbuf, "%H:%M:%S", nowtm);
    snprintf(dbuf, sizeof dbuf, "%s.%06ld", tmpdbuf, tv.tv_usec);
    size_t hex_it;
    fprintf(stderr, "[mcsapi][%s] %s:%d packet hex: ", dbuf, __FILENAME__, __LINE__);
    for (hex_it = 0; hex_it < LEN ; hex_it++)
    {
        fprintf(stderr, "%02X ", (unsigned char)DATA[hex_it]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "[mcsapi][%s] %s:%d printable packet data: ", dbuf, __FILENAME__, __LINE__);
    for (hex_it = 0; hex_it < LEN ; hex_it++)
    {
        if (((unsigned char)DATA[hex_it] < 0x21) or (((unsigned char)DATA[hex_it] > 0x7e)))
        {
            fprintf(stderr, ".");
        }
        else
        {
            fprintf(stderr, "%c", (unsigned char)DATA[hex_it]);
        }
    }
    fprintf(stderr, "\n");
}

