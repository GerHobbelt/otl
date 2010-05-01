#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_FREETDS_ODBC_WORKAROUNDS
#define OTL_ODBC_MSSQL_2005 // Compile OTL 4/ODBC, MS SQL 2005
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
//#define OTL_ODBC_UNIX // Compile OTL 4 / ODBC. Uncomment this when used in Linux / Unix
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tabxx where f1>=:f<int> and f1<=:ff<int>*2",
                 // SELECT statement
              db, // connect object
              otl_explicit_select,
              "SQL Stm #1"
             ); 
   // create select stream
 
 float f1;
 char f2[31];

 i<<8<<8; // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 i<<4<<4; // assigning :f = 4, :ff = 4
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

  db.rlogon("scott/tiger@freetds_mssql"); // connect to ODBC
  select(); // select records from table

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
