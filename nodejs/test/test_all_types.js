/*
 Copyright (c) 2018, MariaDB Corporation. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301  USA
*/

var nodemcsapi = require("../build/Release/nodemcsapi");
var mariasql = require("mariasql");
var assert = require("assert");
var DATABASE_NAME = 'test';

var c = new mariasql({
  host: '127.0.0.1',
  user: 'root',
  db: DATABASE_NAME
});

// test all types
it('test all types', function(done){
  var TABLE_NAME = 'node_all_types';
  c.query('DROP TABLE IF EXISTS ' + TABLE_NAME, function(err, rtn){
    if(err)
      done(err);
    c.query('CREATE TABLE IF NOT EXISTS ' + TABLE_NAME + ' (uint64 bigint unsigned, int64 bigint, uint32 int unsigned, int32 int, uint16 smallint unsigned, int16 smallint, uint8 tinyint unsigned, intEight tinyint, f float, d double, ch4 char(5), vch30 varchar(30), dt date, dtm datetime, dc decimal(18), tx text) ENGINE=columnstore', function(err, rtn){
      if(err)
        done(err);

      // ingest data
      try {
        var d = new nodemcsapi.ColumnStoreDriver();
        var b = d.createBulkInsert(DATABASE_NAME, TABLE_NAME, 0, 0);

        b.setColumn_uint64(0,1);
        b.setColumn_int64(1,2);
        b.setColumn_uint32(2,3);
        b.setColumn_int32(3,4);
        b.setColumn_uint16(4,5);
        b.setColumn_int16(5,6);
        b.setColumn_uint8(6,7);
        b.setColumn_int8(7,8);
        b.setColumn_double(8, 1.234);
        b.setColumn_double(9, 2.34567);
        b.setColumn(10, "ABCD");
        b.setColumn(11, "Hello World");
        b.setColumn(12, new nodemcsapi.ColumnStoreDateTime(2017, 9, 8));
        b.setColumn(13, new nodemcsapi.ColumnStoreDateTime(2017, 9, 8, 13, 58, 23));
        b.setColumn(14, new nodemcsapi.ColumnStoreDecimal(123));
        b.setColumn(15, "Hello World Longer");
        b.writeRow();

        // min values
        b.setColumn_uint64(0, 0);
        b.setColumn_int64(1,Number.MIN_SAFE_INTEGER);
        b.setColumn_uint32(2,0);
        b.setColumn_int32(3,-2147483646);
        b.setColumn_uint16(4,0);
        b.setColumn_int16(5,-32766);
        b.setColumn_uint8(6,0);
        b.setColumn_int8(7,-126);
        b.setColumn_double(8, 1.234);
        b.setColumn_double(9, 2.34567);
        b.setColumn(10, "A");
        b.setColumn(11, "B");
        b.setColumn(12, new nodemcsapi.ColumnStoreDateTime(1000, 1, 1));
        b.setColumn(13, new nodemcsapi.ColumnStoreDateTime(1000, 1, 1, 0, 0, 0));
        b.setColumn(14, new nodemcsapi.ColumnStoreDecimal(-123));
        b.setColumn(15, "C");
        b.writeRow();

        // max values
        b.setColumn_uint64(0,Number.MAX_SAFE_INTEGER);
        b.setColumn_int64(1,Number.MAX_SAFE_INTEGER);
        b.setColumn_uint32(2,4294967293);
        b.setColumn_int32(3, 2147483647);
        b.setColumn_uint16(4, 65533);
        b.setColumn_int16(5,32767);
        b.setColumn_uint8(6,253);
        b.setColumn_int8(7,127);
        b.setColumn_double(8,1.234);
        b.setColumn_double(9, 2.34567);
        b.setColumn(10, "ZYXW");
        b.setColumn(11, "012345678901234567890123456789");
        b.setColumn(12, new nodemcsapi.ColumnStoreDateTime(9999, 12, 31));
        b.setColumn(13, new nodemcsapi.ColumnStoreDateTime(9999, 12, 31, 23, 59, 59));
        b.setColumn(14, new nodemcsapi.ColumnStoreDecimal(123));
        b.setColumn(15, "012345678901234567890123456789");
        b.writeRow();

        b.commit();
      
      } catch (err){
        b.rollback();
        done(err);
      }

      // verify results
      c.query("SELECT * FROM " + TABLE_NAME, null, { useArray: true }, function(err, rows){
        if(err)
          done(err);

        assert.equal(rows[0][0], 1);
        assert.equal(rows[0][1], 2);
        assert.equal(rows[0][2], 3);
        assert.equal(rows[0][3], 4);
        assert.equal(rows[0][4], 5);
        assert.equal(rows[0][5], 6);
        assert.equal(rows[0][6], 7);
        assert.equal(rows[0][7], 8);
        assert.equal(rows[0][8], 1.234);
        assert.equal(rows[0][9], 2.34567);
        assert.equal(rows[0][10], "ABCD");
        assert.equal(rows[0][11], "Hello World");
        assert.equal(rows[0][12], "2017-09-08");
        assert.equal(rows[0][13], "2017-09-08 13:58:23");
        assert.equal(rows[0][14], 123);
        assert.equal(rows[0][15], "Hello World Longer");
        assert.equal(rows[1][0], 0);
        assert.equal(rows[1][1], Number.MIN_SAFE_INTEGER);
        assert.equal(rows[1][2], 0);
        assert.equal(rows[1][3], -2147483646);
        assert.equal(rows[1][4], 0);
        assert.equal(rows[1][5], -32766);
        assert.equal(rows[1][6], 0);
        assert.equal(rows[1][7], -126);
        assert.equal(rows[1][8], 1.234);
        assert.equal(rows[1][9], 2.34567);
        assert.equal(rows[1][10], "A");
        assert.equal(rows[1][11], "B");
        assert.equal(rows[1][12], "1000-01-01");
        assert.equal(rows[1][13], "1000-01-01 00:00:00");
        assert.equal(rows[1][14], -123);
        assert.equal(rows[1][15], "C");
        assert.equal(rows[2][0], Number.MAX_SAFE_INTEGER);
        assert.equal(rows[2][1], Number.MAX_SAFE_INTEGER);
        assert.equal(rows[2][2], 4294967293);
        assert.equal(rows[2][3], 2147483647);
        assert.equal(rows[2][4], 65533);
        assert.equal(rows[2][5], 32767);
        assert.equal(rows[2][6], 253);
        assert.equal(rows[2][7], 127);
        assert.equal(rows[2][8], 1.234);
        assert.equal(rows[2][9], 2.34567);
        assert.equal(rows[2][10], "ZYXW");
        assert.equal(rows[2][11], "012345678901234567890123456789");
        assert.equal(rows[2][12], "9999-12-31");
        assert.equal(rows[2][13], "9999-12-31 23:59:59");
        assert.equal(rows[2][14], 123);
        assert.equal(rows[2][15], "012345678901234567890123456789");
      
        // cleaning up
        c.query('DROP TABLE IF EXISTS ' + TABLE_NAME, function(err, rtn){
          if(err)
            done(err);
        });
        c.end();
        done();
      });
    });
  });
}).timeout(20000);
