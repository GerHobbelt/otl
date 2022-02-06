#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#define OTL_MAP_SQL_GUID_TO_CHAR
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(5, // buffer size
              "insert into test_tab values(:f1<int>,newid())", 
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=13;++i)
  o<<i;
}

void select()
{ 
 otl_stream i(5, // buffer size
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream

 otl_stream o(7, // buffer size
              "insert into test_tab2 values(:f1<int>,:f2<char[37]>)",
                 // INSERT statement
              db // connect object
             ); 
   // create insert stream
 o.set_commit(0); // turnin off the otl_stream's autocommit flag
 
 int f1=0;
 char f2[37];

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
  o<<f1<<f2;
 }

 o.flush(); // flushing the otl_stream's buffer
 db.commit(); // committing transaction

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=mssql2008"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 uniqueidentifier)"
    );  // create table

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab2",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab2(f1 int, f2 uniqueidentifier)"
    );  // create table


  insert(); // insert records into table
  select(); // select records from test_tab and insert them into test_tab2

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
