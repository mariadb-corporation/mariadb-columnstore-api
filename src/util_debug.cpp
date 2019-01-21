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

static uint8_t debugging_level = 0;

void mcsdebug_set(uint8_t level)
{
    debugging_level = level;
}

uint8_t mcsdebug_get()
{
    return debugging_level;
}

void mcsdebug_impl(const char* MSG, const char* file, size_t line, va_list argptr)
{
    if (!debugging_level)
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
    fprintf(stderr, "[mcsapi][%s] %s:%lu ", dbuf,  file, line);
    vfprintf(stderr, MSG, argptr);
    fprintf(stderr, "\n");
}


void mcsdebug_hex_impl(const char* DATA, size_t LEN, const char* file, size_t line)
{
    if (!debugging_level)
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
    fprintf(stderr, "[mcsapi][%s] %s:%lu packet hex: ", dbuf, file, line);
    for (hex_it = 0; hex_it < LEN ; hex_it++)
    {
        if ((debugging_level == 1) && (hex_it > 512))
        {
            fprintf(stderr, "...");
            break;
        }
        fprintf(stderr, "%02X ", (unsigned char)DATA[hex_it]);
    }
    fprintf(stderr, "\n");
    if (debugging_level == 2)
    {
        fprintf(stderr, "[mcsapi][%s] %s:%lu printable packet data: ", dbuf, file, line);
        for (hex_it = 0; hex_it < LEN ; hex_it++)
        {
            if (((unsigned char)DATA[hex_it] < 0x21) || (((unsigned char)DATA[hex_it] > 0x7e)))
            {
                fprintf(stderr, ".");
            }
            else
            {
                fprintf(stderr, "%c", (unsigned char)DATA[hex_it]);
            }
        }
    }
    fprintf(stderr, "\n");
}
