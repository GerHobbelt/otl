#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <stdio.h>
using namespace std;

// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#define OTL_ODBC_ALTERNATE_RPC
#if !defined(_WIN32) && !defined(_WIN64)
#define OTL_ODBC
#else 
#define OTL_ODBC_POSTGRESQL // required with PG ODBC on Windows
#endif
#include <otlv4.h> // include the OTL 4.0 header file


#include <otlv4.h> // include the OTL 4.0 header file


otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(10, // stream buffer size in logical rows.
              "insert into test_tab "
              "values(:f1<int>,:f2<char[31]>)",
                 // INSERT statement
              db // connect object
             );
  // Write rows into the stream. At the end of each row, check for
  // "end-of-row" condition.
  otl_write_row(o,1,"Line1");
  otl_write_row(o,2,"Line21");

  // the old way (operators >>() / <<()) is available as always:
  // o<<1<<"Line1";
  // o<<2<<"Line21";

  o.flush();
}

void upsert(void)
// "upsert" rows into table
{ 
 otl_stream o(10, // stream buffer size in logical rows.
              "insert into test_tab (f1,f2) "
              "values(:f1<int>,:f2<char[31]>) "
              "on conflict (f1) "
              "do update set f2=:f22<char[31]>||'--'||excluded.f1 ",
                 // "UPSERT" statement
              db // connect object
             );
 // Write rows into the stream. At the end of each row, check for
 // "end-of-row" condition.
 otl_write_row(o,2,"Line22","Line22");
 otl_write_row(o,3,"Line3","Line3");

 // the old way (operators >>() / <<()) is available as always:
 // o<<1<<"Line1";
 // o<<2<<"Line21";

 o.flush();
}


void select(void)
{ 
 otl_stream i(5, // stream buffer size in logical rows
              "select * from test_tab ",
                 // SELECT statement
              db // connect object
             ); 
// SELECT output columns
 int f1=0;
 char f2[31];

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
   // Read a row from the stream. Check for the "end-of-row" condition
   // at the end each row.
   otl_read_row(it,f1,f2);
   
   // The old way (operators >>() / <<()) is available as always:
   // it>>f1>>f2;
   
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 
}
 
}


int main()
{
 otl_connect::otl_initialize(); // initialize database API environment
 try{

  db.rlogon("scott/tiger@postgresql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab (f1 int, f2 varchar(30))"
    );  // create table

  db<<"create unique index ind001 on test_tab(f1)";
     // create unique index

  insert(); // insert records into table
  upsert(); // merge records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
