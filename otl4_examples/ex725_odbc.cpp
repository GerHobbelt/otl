#include <iostream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OTL_ODBC_TIMESTEN_WIN // Compile OTL 4.0/TimesTen ODBC for Windows
// Uncomment the following when building this example in Unix/Linux
// #define OTL_ODBC_TIMESTEN_UNIX // Compile OTL 4.0/TimesTen ODBC for Unix

#if (defined(_MSC_VER)&&(_MSC_VER==1200)) // vc++ 6.0
#define OTL_BIGINT __int64
#else
#define OTL_BIGINT long long
#endif


#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream
    o(50, // stream buffer size
      "insert into test_tab values(12345678901230+:f1<bigint>,:f2<char[31]>)", 
          // SQL statement
      db  // connect object
     );
 for(OTL_BIGINT i=1;i<=100;++i){
  o<<i<<"NameXXX";
 }
}

void select()

{ 
 otl_stream i(10, // buffer size can be set to a value in [1..128] range
              "select * from test_tab "
              "where f1>=12345678901230+:f1<bigint> "
              "  and f1<=12345678901230+:f1*2", 
               // the same bind variable can be referenced more than
               // once in TimesTen ODBC
              db // connect object
             ); 
   // create select stream
 
 OTL_BIGINT f1;
 char f2[31];

 i<<(OTL_BIGINT)8; // Writing input values into the stream
 while(!i.eof()){ // while not end-of-data
  i>>f1;
  i>>f2;
#if (defined(_MSC_VER)&&(_MSC_VER==1200)) // vc++ 6.0
  char f1_str[40];
  _i64toa(f1,f1_str,10);
  cout<<"f1="<<f1_str<<", f2=";
#else
  cout<<"f1="<<f1<<", f2=";
#endif
  cout<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize TimesTen ODBC environment
 try{

  db.rlogon("scott/tiger@TT_tt1121_32"); 

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 tt_bigint, f2 varchar(30))"
    );  // create table

  insert(); // insert records into the table
  select(); // select records from the table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
