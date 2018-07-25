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

#define __STDC_FORMAT_MACROS 1

#include <string>
#include <limits>
#include <map>
#include <cstring>
#include <sys/types.h>
#include <inttypes.h>
#include <cmath>
#include <ctime>

#ifdef __linux__
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include "WIN32/time.h"
#include <io.h>
#include <process.h>
#endif

#include <uv.h>
#include <snappy.h>

#include <libmcsapi/mcsapi.h>

#include "util_debug.h"
#include "util_structures.h"
#include "util_messaging.h"
#include "util_network.h"
#include "util_commands.h"
#include "util_dataconvert.h"
