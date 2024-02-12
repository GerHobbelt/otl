#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC_TIMESTEN_WIN
// #define OTL_ODBC_TIMESTEN_UNIX

#include <otlv4.h> // include the OTL 4.0 header file


otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream 
   o(10, // buffer size
     "insert into test_tab "
     "values(:f1<int>,:f4<timestamp>)", 
        // INSERT statement
     db // connect object
    );

 otl_datetime tm3;

  for(int i=1;i<=10;++i){

  tm3.year=1998;
  tm3.month=10;
  tm3.day=19;
  tm3.hour=23;
  tm3.minute=12;
  tm3.second=12;

  o<<i<<tm3;

 }
 
}

void select(void)
{ 
 otl_stream i(10, // stream buffer size in rows
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
 
 int f1=0;
 otl_datetime f4;

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f4;
  cout<<"f1="<<f1;
  cout<<", f4="<<f4.month<<"/"<<f4.day<<"/"
      <<f4.year<<" "<<f4.hour<<":"<<f4.minute<<":"
      <<f4.second;
  cout<<endl;
 }
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@TT_tt1122_32"); 

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f4 timestamp)"
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
