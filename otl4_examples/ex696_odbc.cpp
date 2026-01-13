#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

unsigned int my_trace_level=
   0x1 | // 1st level of tracing
   0x2 | // 2nd level of tracing
   0x4 | // 3rd level of tracing
   0x8 | // 4th level of tracing
   0x10; // 5th level of tracing
// each level of tracing is represented by its own bit, 
// so levels of tracing can be combined in an arbitrary order.

#define OTL_TRACE_LEVEL my_trace_level 
   // enables OTL tracing, and uses my_trace_level as a trace control variable.

#define OTL_TRACE_STREAM cerr 
   // directs all OTL tracing to cerr

#define OTL_TRACE_LINE_PREFIX "MY OTL TRACE ==> " 
   // redefines the default OTL trace line prefix. This #define is optional

// change the date format in OTL tracing to YYYY-MM-DD.
#define OTL_TRACE_FORMAT_DATETIME(s)                                \
      s.year<<"-"<<s.month<<"-"<<s.day                              \
      <<" "<<s.hour<<":"<<s.minute<<":"<<s.second<<"."<<s.fraction 

// the following #define is needed in case if
// timestamps with time zones are used.
#define OTL_TRACE_FORMAT_TZ_DATETIME(s)                            \
      s.year<<"-"<<s.month<<"-"<<s.day                             \
      <<" "<<s.hour<<":"<<s.minute<<":"<<s.second<<"."<<s.fraction \
      <<" "<<s.tz_hour<<":"<<s.tz_minute


      

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4.0/MS SQL 2008 SNAC
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab2 values(:f1<int>,:f2<timestamp>)", 
                 // SQL statement
              db // connect object
             );

 otl_datetime tm;

 for(int i=1;i<=10;++i){
  tm.year=2009;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  o<<i<<tm;
 }
}

void select(void)
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab2 where f2=:f2<timestamp>",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 otl_datetime tm,f2;

 tm.year=2009;
 tm.month=10;
 tm.day=19;
 tm.hour=23;
 tm.minute=12;
 tm.second=12;

 i<<tm; // assigning :1 = tm
   // SELECT automatically executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2.year<<"-"<<f2.month<<"-"<<f2.day
      <<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second<<endl;
 
}
 
}

int main()
{
 otl_connect::otl_initialize();

 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab2",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab2(f1 int, f2 datetime)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
