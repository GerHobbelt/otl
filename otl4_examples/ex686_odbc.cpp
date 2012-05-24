#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_UNICODE // enable Unicode (UTF-16)

// Uncomment the line below if you use #define OTL_UNICODE_CHAR_TYPE
//#define OTL_UNICODE_CHAR_TYPE wchar_t

// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#define OTL_ODBC_ALTERNATE_RPC
#if !defined(_WIN32) && !defined(_WIN64)
#define OTL_ODBC
#else 
#define OTL_ODBC_POSTGRESQL // required with PG ODBC on Windows
#endif
#define OTL_ODBC_TIME_ZONE
#define OTL_ODBC_STRING_TO_TIMESTAMP(str,tm)            \
{                                                       \
  swscanf(str,                                          \
         L"%04d-%02d-%02d %02d:%02d:%02d.%06ld%hd",     \
         &tm.year,                                      \
         &tm.month,                                     \
         &tm.day,                                       \
         &tm.hour,                                      \
         &tm.minute,                                    \
         &tm.second,                                    \
         &tm.fraction,                                  \
         &tm.tz_hour);                                  \
}

#define OTL_ODBC_TIMESTAMP_TO_STRING(tm,str)            \
{                                                       \
  swprintf(str,                                         \
          L"%04d-%02d-%02d %02d:%02d:%02d.%06ld %+hd",  \
          tm.year,                                      \
          tm.month,                                     \
          tm.day,                                       \
          tm.hour,                                      \
          tm.minute,                                    \
          tm.second,                                    \
          tm.fraction,                                  \
          tm.tz_hour);                                  \
}

#include <otlv4.h> // include the OTL 4.0 header file


otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(50, // PostgreSQL 8.1 and higher, the buffer can be > 1
              "insert into test_tab values(:f1<int>,:f2<char[60]>, :f3<char[60]>)", 
                   // INSERT statement, :f2, and :f3 need to be declared as char[XX]
                   // in order for otl_datetime-to-string conversion to work.
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
  tm.frac_precision=6; // microseconds
  tm.fraction=123456;
  tm.tz_hour=-5; // time zone offset in hours
  tm.tz_minute=0;
  o<<i<<tm<<tm;
 }
}

void select(void)
{ 
 otl_stream i(5, // stream buffer size can be > great 1 on SELECT.
              "select f1, f2\\:\\:varchar, f3\\:\\:varchar from test_tab",
               // f2, f3 need to be converted into varchar/string, 
               // for example: f2::varchar
               // SELECT statement
              db // connect object
             ); 

 int f1;
 otl_datetime f2, f3;

 // Second's precision needs to be specified BEFORE the stucture can be
 // used either for writing or reading timestamp values, which have second's 
 // fractional part.
 f2.frac_precision=6; // milliseconds

 while(!i.eof()){ // while not end-of-data
   i>>f1>>f2>>f3;
   cout<<"f1="<<f1;
   cout<<", f2="<<f2.month<<"/"<<f2.day<<"/"
       <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
       <<f2.second<<"."
       <<f2.fraction;
   if(f2.tz_hour>=0)
       cout<<" +"<<f2.tz_hour;
   else
       cout<<" "<<f2.tz_hour;
   cout<<", f3="<<f3.month<<"/"<<f3.day<<"/"
       <<f3.year<<" "<<f3.hour<<":"<<f3.minute<<":"
       <<f3.second<<"."
       <<f3.fraction;
   cout<<endl;
 }
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@postgresql"); // connect to PostgreSQL

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

   db.commit();
  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 timestamp(6) with time zone, f3 timestamp(6))"
    );  // create table

  db.commit(); // commit DDL statements 

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
