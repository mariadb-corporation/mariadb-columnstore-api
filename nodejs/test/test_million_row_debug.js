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

var nodemcsapi = require("../build/Debug/nodemcsapi");
var mariasql = require("mariasql");
var assert = require("assert");
var DATABASE_NAME = 'test';


var c = new mariasql({
  host: '127.0.0.1',
  user: 'root',
  db: DATABASE_NAME
});


// million row ingestion test
  var TABLE_NAME = 'node_million_row';
  c.query('DROP TABLE IF EXISTS ' + TABLE_NAME, function(err, rtn){
    c.query('CREATE TABLE IF NOT EXISTS ' + TABLE_NAME + ' (a int, b int) ENGINE=columnstore', function(err, rtn){
      // million row test
      var num_rows = 1000000;
      
      try{
        var d = new nodemcsapi.ColumnStoreDriver();
	d.setDebug(true);
        var b = d.createBulkInsert(DATABASE_NAME, TABLE_NAME, 0, 0);
	var ad = false;

        for(var i=0; i<num_rows; i++){
	  if(ad == false && i> 12000){
             ad = true;
             d.setDebugAdvanced(true);
          }
	  console.log("call setColumn 0 " + i);
          b.setColumn_int32(0, i);
	  console.log("call setColumn 1 " + i);
          b.setColumn_int32(1, num_rows-i);
	  console.log("call writeRow " + i);
          b.writeRow();
        }
	console.log("call commit");
        b.commit();
      }catch (err){
        console.log("call rollback");
        b.rollback();
	console.log(err.toString());
        process.exit(1);
      }


      // verify results
      c.query("SELECT COUNT(*) FROM " + TABLE_NAME, null, { useArray: true }, function(err, rows){
        
        assert.equal(rows[0][0], num_rows);
      
        // cleaning up
        c.query('DROP TABLE IF EXISTS ' + TABLE_NAME, function(err, rtn){
        });
        c.end();
      });
    });
});
