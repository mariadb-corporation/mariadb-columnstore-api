#!/bin/python
import pymcsapi, pytest, datetime
import mysql.connector as mariadb

DB_NAME = 'mcsapi'

def create_db():
    try:
        conn = mariadb.connect(user='root')
        cursor = conn.cursor();
        cursor.execute("CREATE DATABASE IF NOT EXISTS %s" %(DB_NAME,))
    except mariadb.Error as err:
        pytest.fail("Error creating database %s" %(err,))
    finally:
        if cursor: cursor.close()
        if conn: conn.close()

def create_conn():
    create_db()    
    try:
        return mariadb.connect(user='root', database=DB_NAME)
    except mariadb.Error as err:
        pytest.fail("Error connecting to mcsapi database %s" %(err,))        

def exec_stmt(conn, stmt):
    try:
        cursor = conn.cursor()
        cursor.execute(stmt)
    except mariadb.Error as err:
        pytest.fail("Error executing statement: %s, error: %s" %(stmt,err))
    finally:
        if cursor: cursor.close()

def drop_table(conn, tablename):
    exec_stmt(conn, "DROP TABLE IF EXISTS %s" %(tablename,))
   
def all_types_validate(conn, rowid, expected):
    query_all_types = "select uint64, int64, uint32, int32, uint16, int16, uint8, `int8`, f, d, ch4, vch30, dt, dtm, dc, tx from pymcsapi_all_types where uint64 = %s"
    try:
        cursor = conn.cursor()
        cursor.execute(query_all_types, (rowid,))
        for (uint64, int64, uint32, int32, uint16, int16, uint8, int8, f, d, ch4, vch30, dt, dtm, dc, tx) in cursor:
            rowStr = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}".format(uint64, int64, uint32, int32, uint16, int16, uint8, int8, f,d, ch4, vch30, dt, dtm, dc, tx)
            assert rowStr == expected
    except mariadb.Error as err:
        pytest.fail("Error executing query: %s, error: %s" %(query_all_types,err))
    finally:
        if cursor: cursor.close()
   
#
# Test all column types and min / max range values
#
def test_all_types():
    # create connection for table creation / query
    conn = create_conn()

    # create all_types table
    tablename = 'pymcsapi_all_types'
    drop_table(conn, tablename)
    create_all_types = """create table %s (
    uint64 bigint unsigned,
    int64 bigint,
    uint32 int unsigned,
    int32 int,
    uint16 smallint unsigned,
    int16 smallint,
    uint8 tinyint unsigned,
    `int8` tinyint,
    f float,
    d double,
    ch4 char(5),
    vch30 varchar(30),
    dt date,
    dtm datetime,
    dc decimal(18),
    tx text
    ) engine=columnstore""" % (tablename,)
    exec_stmt(conn, create_all_types)

    # load rows into all_types table
    d = pymcsapi.ColumnStoreDriver()
    # simple values
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    try:
        b.setColumn(0, long(1))
        b.setColumn(1,long(2))
        b.setColumn(2,int(3))
        b.setColumn(3,int(4))
        b.setColumn(4,int(5))
        b.setColumn(5,int(6))
        b.setColumn(6,int(7))
        b.setColumn(7,int(8))
        b.setColumn(8,float(1.234))
        b.setColumn(9, float(2.34567))
        b.setColumn(10, 'ABCD')
        b.setColumn(11, 'Hello World')
        b.setColumn(12, pymcsapi.ColumnStoreDateTime(2017, 9, 8))
        b.setColumn(13, pymcsapi.ColumnStoreDateTime(2017, 9, 8, 13, 58, 23))
        b.setColumn(14, pymcsapi.ColumnStoreDecimal(123)) 
        b.setColumn(15, 'Hello World Longer')
        b.writeRow()
        
        # min values
        b.setColumn(0, long(0))
        b.setColumn(1,long(-9223372036854775806))
        b.setColumn(2,int(0))
        b.setColumn(3,int(-2147483646))
        b.setColumn(4,int(0))
        b.setColumn(5,int(-32766))
        b.setColumn(6,int(0))
        b.setColumn(7,int(-126))
        b.setColumn(8,float(1.234))
        b.setColumn(9, float(2.34567))
        b.setColumn(10, 'A')
        b.setColumn(11, 'B')
        b.setColumn(12, pymcsapi.ColumnStoreDateTime(1000, 1, 1))
        b.setColumn(13, pymcsapi.ColumnStoreDateTime(1000, 1, 1, 0, 0, 0))
        b.setColumn(14, pymcsapi.ColumnStoreDecimal(-123))
        b.setColumn(15, 'C')
        b.writeRow()

        # max values
        b.setColumn(0, long(9223372036854775807))
        b.setColumn(1,long(9223372036854775807)) # python long is signed
        b.setColumn(2,int(4294967293))
        b.setColumn(3,int(2147483647))
        b.setColumn(4,int(65533))
        b.setColumn(5,int(32767))
        b.setColumn(6,int(253))
        b.setColumn(7,int(127))
        b.setColumn(8,float(1.234))
        b.setColumn(9, float(2.34567))
        b.setColumn(10, 'ZYXW')
        b.setColumn(11, '012345678901234567890123456789')
        b.setColumn(12, pymcsapi.ColumnStoreDateTime(9999, 12, 31))
        b.setColumn(13, pymcsapi.ColumnStoreDateTime(9999, 12, 31, 23, 59, 59))
        b.setColumn(14, pymcsapi.ColumnStoreDecimal(123))
        b.setColumn(15, '012345678901234567890123456789')
        b.writeRow()
        
        b.commit()
    except RuntimeError as err:
        b.rollback()
        pytest.fail("Error executing bulk insert: %s" % (err,)) 

    # verify data
    all_types_validate(conn, 1, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.34567, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23, 123, Hello World Longer")
    all_types_validate(conn, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 1000-01-01, 1000-01-01 00:00:00, -123, C")
    all_types_validate(conn, 9223372036854775807, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59, 123, 012345678901234567890123456789")
    
    drop_table(conn, tablename)
    conn.close()

 
#
# Test creation of date/datetime values from 2016-01-01 to 2016-12-31 for both date and datetime columnstore
#
def test_dates():
     # setup / create test table
    conn = create_conn()
    tablename = 'pymcsapi_dates'
    drop_table(conn, tablename)
    exec_stmt(conn, "create table %s (id int, dt date, dtm datetime) engine=columnstore" % (tablename,))
    
    # generate and load a range of dates
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    maxDays = 366  + 365 + 1;
    dt = datetime.datetime(2016, 1, 1, 12, 34, 56)
    try: 
        for i in xrange(1,maxDays):
            b.setColumn(0, i)
            b.setColumn(1, pymcsapi.ColumnStoreDateTime(dt.year, dt.month, dt.day))
            b.setColumn(2, pymcsapi.ColumnStoreDateTime(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second))
            b.writeRow()
            dt += datetime.timedelta(days=1)
        b.commit()
    except RuntimeError as err:
        b.rollback()
        pytest.fail("Error executing bulk insert: %s" %(err,)) 

    
    # query and validate values written
    try:
        cursor = conn.cursor()
        cursor.execute("select id, dt, dtm from pymcsapi_dates order by id")
        exp_i = 1
        exp_dt = datetime.date(2016, 1, 1)
        exp_dtm = datetime.datetime(2016, 1, 1, 12, 34, 56)
        for (id, dt, dtm) in cursor:
            assert id == exp_i
            assert dt == exp_dt
            assert dtm == exp_dtm
            exp_i += 1
            exp_dt += datetime.timedelta(days=1)
            exp_dtm += datetime.timedelta(days=1) 
    except mariadb.Error as err:
        pytest.fail("Error executing test_dates validation query: %s" %(err,))
    finally:
        if cursor: cursor.close()
    
    drop_table(conn, tablename)
    conn.close()
    
def i1_common(datatype, ch_len):
    conn = create_conn()
    tablename = 'pymcsapi_i1'
    drop_table(conn, tablename)
    if (datatype == 'char'):
        exec_stmt(conn, 'create table pymcsapi_i1(i int, ch char(%s)) engine=columnstore' % (ch_len,))
    else:
        exec_stmt(conn, 'create table pymcsapi_i1(i int, ch varchar(%s)) engine=columnstore' %(ch_len,))        
    
    d = pymcsapi.ColumnStoreDriver()
    b = d.createBulkInsert(DB_NAME, tablename, 0, 0)
    try:
        b.setColumn(0,1).setColumn(1, 'ABC').writeRow()
        b.setColumn(0,2).setColumn(1, 'A').writeRow()
        b.setColumn(0,3).setColumn(1, 'XYZ').writeRow()
        b.commit()
    except RuntimeError as err:
        b.rollback()
        pytest.fail("Error executing bulk insert: %s" %(err,)) 
        
    try:
        cursor = conn.cursor()
        cursor.execute('select i, ch from pymcsapi_i1 order by i')
        for (i, ch) in cursor:
            if (i == 1): assert ch == 'ABC'
            elif (i == 2): assert ch == 'A'
            elif (i == 3): assert ch == 'XYZ'
            else: pytest.fail("invalid i value: %s" %(i,))
    except mariadb.Error as err:
        pytest.fail("Error executing i1_common query: %s" %(err,))
    finally:
        if cursor: cursor.close()

    drop_table(conn, tablename)   
    conn.close()

def  test_i1_ch9():
    i1_common('char', 9)
    
def  test_i1_ch8():
    i1_common('char', 8)

def  test_i1_ch7():
    i1_common('char', 7)
    
def  test_i1_ch6():
    i1_common('char', 6)
    
def  test_i1_ch5():
    i1_common('char', 5)
    
def  test_i1_ch4():
    i1_common('char', 4)
    
def  test_i1_ch3():
    i1_common('char', 3)
    
def  test_i1_vch9():
    i1_common('varchar', 9)
    
def  test_i1_vch8():
    i1_common('varchar', 8)

def  test_i1_vch7():
    i1_common('varchar', 7)
    
def  test_i1_vch6():
    i1_common('varchar', 6)
    
def  test_i1_vch5():
    i1_common('varchar', 5)
    
def  test_i1_vch4():
    i1_common('varchar', 4)
    
def  test_i1_vch3():
    i1_common('varchar', 3)

