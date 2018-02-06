#!/usr/bin/python

# Copyright (c) 2018, MariaDB Corporation. All rights reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA

import pytest
import pymcsapi

def test_exception_thrown():
    with pytest.raises(RuntimeError) as exceptionInfo:
        driver = pymcsapi.ColumnStoreDriver()
        catalog = driver.getSystemCatalog()
        table = catalog.getTable("test", "nonexistent")
    assert exceptionInfo.value.args[0] == 'test.nonexistent not found'

test_exception_thrown()
