#include <iostream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<i<<tmp;
 }
}

void select(const int af1)
{ 
 otl_stream i(50, // buffer size
              "{call prc1(:f1<int,in>)}",
                 // Stored Proc Call
              db, // connect object
              otl_implicit_select // implicit result set / select
             ); 
   // create select stream
 
 int f1;
 char f2[31];

 i<<af1; // Writing a input value into the stream
 while(!i.eof()){ // while not end-of-data
  i>>f1;
  cout<<"f1="<<f1<<", f2=";
  i>>f2;
  if(i.is_null())
   cout<<"NULL";
  else
   cout<<f2;
  cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

 db.rlogon("scott/tiger@sapdbsql"); // connect to database

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
    "DROP PROCEDURE prc1",
    otl_exception::disabled // disable OTL exceptions
   ); 

  otl_cursor::direct_exec
   (
    db,
    "CREATE DBPROC prc1 (IN f1 int) "
    "RETURNS CURSOR AS "
    "$CURSOR = 'CUR1'; "
    "DECLARE :$CURSOR CURSOR FOR "
    "SELECT * FROM scott.test_tab WHERE f1 >= :f1 and f1 <= :f1*2; "
   );

  insert(); // insert records into the table
  select(8); // select records from the table

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
