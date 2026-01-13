#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC_MYSQL // Compile OTL 4/MyODBC
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(1, // buffer size should be == 1 on INSERT
              "insert into test_tab2 "
              "values(:f1<int>,:f2<timestamp>,:f3<timestamp>)", 
                 // INSERT statement
              db // connect object
             );

 otl_datetime tm;

 for(int i=1;i<=10;++i){
  tm.year=1998;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  o<<i<<tm<<tm;
 }
}

void select(void)
{ 
 otl_stream i(50, // buffer size may be > 1
              "select * from test_tab2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 otl_datetime f2,f3;

#if (defined(_MSC_VER) && _MSC_VER>=1600) || defined(OTL_CPP_11_ON)
// C++11 (or higher) compiler
 for(auto& it : i){
  it>>f1>>f2>>f3;
  cout<<"f1="<<f1;
  cout<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second;
  cout<<", f3="<<f3.month<<"/"<<f3.day<<"/"
      <<f3.year<<" "<<f3.hour<<":"<<f3.minute<<":"
      <<f3.second;
  cout<<endl;
 }
#else
// C++98/03 compiler
 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2>>f3;
  cout<<"f1="<<f1;
  cout<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second;
  cout<<", f3="<<f3.month<<"/"<<f3.day<<"/"
      <<f3.year<<" "<<f3.hour<<":"<<f3.minute<<":"
      <<f3.second;
  cout<<endl;
 
}
#endif
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@mysql"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab2",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab2(f1 int, f2 datetime, f3 date)"
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
