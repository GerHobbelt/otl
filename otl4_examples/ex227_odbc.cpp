#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0
// The following #define is required with MyODBC 3.51.11 and higher
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(1, // buffer size should be == 1 on INSERT
              "insert into test_tab2 values "
              "(:f1<int>,:f2<timestamp>,:f3<timestamp>), "
              "(:f12<int>,:f22<timestamp>,:f32<timestamp>), "
              "(:f13<int>,:f23<timestamp>,:f33<timestamp>), "
              "(:f14<int>,:f24<timestamp>,:f34<timestamp>), "
              "(:f15<int>,:f25<timestamp>,:f35<timestamp>) ", 
// INSERT statement. Multiple sets of values can be used
// to work around the lack of the bulk interface
              db // connect object
             );

// If the number of rows to be inserted is not known in advance,
// another stream with the same INSERT can be opened
 otl_stream o2(1, // buffer size should be == 1 on INSERT
              "insert into test_tab2 values "
               "(:f1<int>,:f2<timestamp>,:f3<timestamp>)",
                 // INSERT statement
              db // connect object
             );
 otl_datetime tm;
 int i;
 for(i=1;i<=10;++i){
  tm.year=1998;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  o<<i<<tm<<tm;
 }
 for(i=11;i<=13;++i){
  tm.year=1998;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  o2<<i<<tm<<tm;
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
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@mysql35"); // connect to ODBC

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
