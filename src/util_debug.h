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

#if DEBUG
#define mcsdebug(MSG, ...) do { \
    struct timeval tv; \
    time_t nowtime; \
    struct tm *nowtm; \
    char tmpdbuf[64], dbuf[64]; \
    gettimeofday(&tv, NULL); \
    nowtime = tv.tv_sec; \
    nowtm = localtime(&nowtime); \
    strftime(tmpdbuf, sizeof tmpdbuf, "%H:%M:%S", nowtm); \
    snprintf(dbuf, sizeof dbuf, "%s.%06ld", tmpdbuf, tv.tv_usec); \
    fprintf(stderr, "[mcsapi][%s] %s:%d " MSG "\n", dbuf,  __FILENAME__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define mcsdebug_hex(DATA, LEN) do { \
    struct timeval tv; \
    time_t nowtime; \
    struct tm *nowtm; \
    char tmpdbuf[64], dbuf[64]; \
    gettimeofday(&tv, NULL); \
    nowtime = tv.tv_sec; \
    nowtm = localtime(&nowtime); \
    strftime(tmpdbuf, sizeof tmpdbuf, "%H:%M:%S", nowtm); \
    snprintf(dbuf, sizeof dbuf, "%s.%06ld", tmpdbuf, tv.tv_usec); \
    size_t hex_it; \
    fprintf(stderr, "[mcsapi][%s] %s:%d packet hex: ", dbuf, __FILENAME__, __LINE__); \
    for (hex_it = 0; hex_it < LEN ; hex_it++) \
    { \
        fprintf(stderr, "%02X ", (unsigned char)DATA[hex_it]); \
    } \
    fprintf(stderr, "\n"); \
    fprintf(stderr, "[mcsapi][%s] %s:%d printable packet data: ", dbuf, __FILENAME__, __LINE__); \
    for (hex_it = 0; hex_it < LEN ; hex_it++) \
    { \
        if (((unsigned char)DATA[hex_it] < 0x21) or (((unsigned char)DATA[hex_it] > 0x7e))) \
        { \
            fprintf(stderr, "."); \
        } \
        else \
        { \
            fprintf(stderr, "%c", (unsigned char)DATA[hex_it]); \
        } \
    } \
    fprintf(stderr, "\n"); \
} while(0)
#else
#define mcsdebug(MSG, ...)
#define mcsdebug_hex(DATA, LEN)
#endif
