#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
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
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
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
  o<<static_cast<float>(i)<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "{call my_proc(:f1<int,in>)}",
                 // implicit SELECT statement
              db, // connect object
              otl_implicit_select // implicit SELECT statement
             ); 
 
 float f1=0;
 char f2[31];

 i<<8; // assigning :f1 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 int n=0;
 cout<<"====> Starting a fetch sequence...."<<endl;
 while(!i.eof()){ // while not end-of-data
  ++n;
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
  if(n>=3){
   // breaking the fetch sequence after 3 rows are fetched, and 
   // "cleaning" the state of the SELECT stream.
   cout<<"Cancelling the fetch sequence after 3 rows are fetched"<<endl;
   i.clean();
   break;
  }
 }

 cout<<"====> Starting another fetch sequence..."<<endl;
 i<<4; // assigning :f1 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("sa/tigger@freetds_sybsql"); // connect to Sybase
  db.auto_commit_off();

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
    "drop procedure my_proc",
    otl_exception::disabled // disable OTL exceptions
   ); // drop old version of stored procedure, if there is one.

  otl_cursor::direct_exec
   (db,
    "CREATE PROCEDURE my_proc "
    "  @F1 int "
    "AS "
    "SELECT * FROM test_tab "
    "WHERE f1>=@F1 AND f1<=@F1*2 "
   );  // create stored procedure


  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from MS SQL Server

 return 0;

}
