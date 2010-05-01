#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
#define OTL_ODBC_TIME_ZONE // enable otl_datime's tz components for ODBC
#define OTL_UNICODE // Enablde Unicode (UTF-16)

// Uncomment the line below if you use #define OTL_UNICODE_CHAR_TYPE
//#define OTL_UNICODE_CHAR_TYPE wchar_t

// The following two #defines need to be defined in order 
// to implement OTL support for datetimeoffset
#define OTL_ODBC_STRING_TO_TIMESTAMP(str,tm)              \
{                                                         \
  swscanf(str,                                            \
           L"%04d-%02d-%02d %02d:%02d:%02d.%07ld %hd:%hd",\
           &tm.year,                                      \
           &tm.month,                                     \
           &tm.day,                                       \
           &tm.hour,                                      \
           &tm.minute,                                    \
           &tm.second,                                    \
           &tm.fraction,                                  \
           &tm.tz_hour,                                   \
           &tm.tz_minute);                                \
}

#define OTL_ODBC_TIMESTAMP_TO_STRING(tm,str)                    \
{                                                               \
  swprintf(str,                                                 \
            L"%04d-%02d-%02d %02d:%02d:%02d.%07ld %+hd:%hd",    \
            tm.year,                                            \
            tm.month,                                           \
            tm.day,                                             \
            tm.hour,                                            \
            tm.minute,                                          \
            tm.second,                                          \
            tm.fraction,                                        \
            tm.tz_hour,                                         \
            tm.tz_minute);                                      \
}

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab2 values(:f1<int>,:f2<char[60]>)", 
               // char[XXX] should be used in binding with datetimeoffset
               // SQL statement
              db // connect object
             );

 otl_datetime tm;

 for(int i=1;i<=10;++i){
  tm.year=2008;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  tm.fraction=1234567;
  tm.frac_precision=7;
// MS SQL 2008's datetimeoffset is "time zone aware", but it doesn't enforce
// time zone validity. -5:37 time zone doesn't exist, yet it's perfectly valid
// for MS SQL 2008. It looks more like a feature than a bug. If you have a problem
// with this feature, report it as a bug to Microsoft.
  tm.tz_hour=-5;
  tm.tz_minute=37;
  o<<i<<tm;
 }
}

void select(void)
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab2 where f2=:f2<char[60]>",
               // char[XXX] should be used in binding with datetimeoffset
               // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 otl_datetime tm,f2;

 tm.year=2008;
 tm.month=10;
 tm.day=19;
 tm.hour=23;
 tm.minute=12;
 tm.second=12;
 tm.fraction=1234567;
 tm.frac_precision=7;
// MS SQL 2008's datetimeoffset is "time zone aware", but it doesn't enforce
// time zone validity. -5:37 time zone doesn't exist, yet it's perfectly valid
// for MS SQL 2008. It looks more like a feature than a bug. If you have a problem
// with this feature, report it as a bug to Microsoft. 
 tm.tz_hour=-5;
 tm.tz_minute=37;

 f2.frac_precision=7;

 i<<tm; // assigning :f1 = tm
   // SELECT automatically executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second<<"."<<f2.fraction;
  if(f2.tz_hour>=0)
    cout<<" +"<<f2.tz_hour;
  else
    cout<<" "<<f2.tz_hour;
  cout<<":"<<f2.tz_minute<<endl;
 }
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab2",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab2(f1 int, f2 datetimeoffset(7))"
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

 db.logoff(); // disconnect from Oracle

 return 0;

}
