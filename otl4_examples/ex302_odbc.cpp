#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
#include <otlv4.h> // include the OTL 4.0 header file


otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream 
   o(1, // buffer size should be set to 1
     "insert into test_tab "
     "values(:f1<int>,:f2<timestamp>)", 
        // INSERT statement
     db // connect object
    );

 otl_datetime tm2;

 for(int i=1;i<=10;++i){
  // fill out a timestamp value (both date and time components)
  tm2.year=1998;
  tm2.month=10;
  tm2.day=19;
  tm2.hour=23;
  tm2.minute=12;
  tm2.second=12;
  o<<i<<tm2;
 }

}

void select(void)
{ 
 otl_stream i(50, // stream buffer size can be > greater than 1 on SELECT.
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
 
 int f1=0;
 otl_datetime f2;

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1;
  cout<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second;
  cout<<endl;
 }
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@msaccess");

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 datetime)"
    );  // create table

  insert(); // insert records into table
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
