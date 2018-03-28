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


// million row ingestion test
it('million row char ingestion test', function(done){
  var TABLE_NAME = 'node_million_row_char';
  c.query('DROP TABLE IF EXISTS ' + TABLE_NAME, function(err, rtn){
    if(err)
      done(err);
    c.query('CREATE TABLE IF NOT EXISTS ' + TABLE_NAME + ' (a char(1), b char(1)) ENGINE=columnstore', function(err, rtn){
      if(err)
        done(err);

      // million row test
      var num_rows = 1000000;

      try{
        var d = new nodemcsapi.ColumnStoreDriver();
        var b = d.createBulkInsert(DATABASE_NAME, TABLE_NAME, 0, 0);
        
        for(var i=0; i<num_rows; i++){
          b.setColumn(0, "i");
          b.setColumn(1, "o");
          b.writeRow();
        }
        b.commit();
      }catch (err){
        b.rollback();
        done(err);
      }

      // verify results
      c.query("SELECT COUNT(*) FROM " + TABLE_NAME, null, { useArray: true }, function(err, rows){
        if(err)
          done(err);
        
        assert.equal(rows[0][0], num_rows);
      
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
