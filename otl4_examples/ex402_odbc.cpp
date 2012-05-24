#include <iostream>
using namespace std;

#include <stdio.h>

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__CYGWIN__)
#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC
#else 
#define OTL_ODBC
#endif
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );

 o.set_commit(0); // set stream's auto-commit to OFF.

 try{
  o<<1<<"Line1"; // Enter one row into the stream
  o.flush(); // flush the strem buffer, i.e. force
             // the stream to execute
  o<<1<<"Line1"; // Enter the same data into the stream
                 // and cause a "duplicate key" error.
  o.flush();
 }catch(otl_exception& p){
  if(p.code==2601){
   // ... duplicate key ...
   o.clean(1); // clean up the stream's buffer
               // and clean up the stream's internal
               // error flag as well. By doing this, 
               // it's possible to recover from 
               // a database error without closing
               // the stream. Remember, the number of 
               // situtation when it's possible is 
               // limited and the recover procedure should
               // be carefully designed.
  }else
   throw; // re-throw the exception to the outer catch block.
 }


 o<<2<<"Line2"; // Enter one more row of data after
                // recovering from the "duplicate key" 
                // error
 o.flush();

 db.commit(); // commit transaction

}

void select()
{ 
 otl_stream i(10, // buffer size
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 char f2[31];

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tigger@sybsql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  otl_cursor::direct_exec
   (
    db,
    "create unique index ind001 on test_tab(f1)"
    );  // create unique index 

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.code<<endl; // print out error code
  cerr<<p.sqlstate<<endl; // print out error SQLSTATE
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
