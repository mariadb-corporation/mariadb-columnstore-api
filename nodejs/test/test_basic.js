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


// basic ingestion test
it('basic ingestion test', function(done){
  var TABLE_NAME = 'node_basic';
  c.query('DROP TABLE IF EXISTS ' + TABLE_NAME, function(err, rtn){
    if(err)
      done(err);
    c.query('CREATE TABLE IF NOT EXISTS ' + TABLE_NAME + ' (i int, c char(3)) ENGINE=columnstore', function(err, rtn){
      if(err)
        done(err);

      // simple 1 row test
      try{
        var d = new nodemcsapi.ColumnStoreDriver();
        var b = d.createBulkInsert(DATABASE_NAME, TABLE_NAME, 0, 0);

        b.setColumn_int32(0, 1);
        b.setColumn(1, "jkl");
        b.writeRow();
        b.commit();
      }catch (err){
        b.rollback();
        done(err);
      }

      // verify results
      c.query("SELECT i, c FROM " + TABLE_NAME, null, { useArray: true }, function(err, rows){
        if(err)
          done(err);
      
        assert.equal(rows[0][0], 1);
        assert.equal(rows[0][1], "jkl");
      
        // basic system catalog tests
        var ctlg = d.getSystemCatalog();
        var table = ctlg.getTable(DATABASE_NAME, TABLE_NAME);
        assert(table != null);
        assert.equal(DATABASE_NAME, table.getSchemaName());
        assert.equal(TABLE_NAME, table.getTableName());
        assert.equal(2, table.getColumnCount());
        assert(table.getOID() >= 3000);
        var column1 = table.getColumn(0);
        assert.equal("i", column1.getColumnName());
        assert.equal(nodemcsapi.DATA_TYPE_INT, column1.getType());
        assert(true == column1.isNullable());
        var column2 = table.getColumn(1);
        assert.equal("c", column2.getColumnName());
        assert.equal(nodemcsapi.DATA_TYPE_CHAR, column2.getType());
        assert.equal(3, column2.getWidth());
        assert(true == column2.isNullable());

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
