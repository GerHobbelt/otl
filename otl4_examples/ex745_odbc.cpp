#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#define OTL_ODBC_ALTERNATE_RPC
#if !defined(_WIN32) && !defined(_WIN64)
#define OTL_ODBC
#else 
#define OTL_ODBC_POSTGRESQL // required with PG ODBC on Windows
#endif
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

// Sample UTF8 based string
unsigned char utf8_sample[]=
{0x61,0x62,0x63,0xd0,0x9e,0xd0,0x9b,0xd0,
 0xac,0xd0,0x93,0xd0,0x90,0x0};

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );

 unsigned char tmp[31];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
   strcpy_s(reinterpret_cast<char*>(tmp),
            sizeof(tmp),
            reinterpret_cast<const char*>(utf8_sample));
#else
   strcpy(reinterpret_cast<char*>(tmp),reinterpret_cast<const char*>(utf8_sample));
#endif
#else
   strcpy(reinterpret_cast<char*>(tmp),reinterpret_cast<const char*>(utf8_sample));
#endif
   o<<i;
   o<<tmp;
 }

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 unsigned char f2[31]={0};

 i<<8<<8; // assigning :f11 = 8, : :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
   i>>f1;
   i>>f2;
   cout<<"f1="<<f1<<", f2=";
   for(int j=0;f2[j]!=0;++j)
     printf("%2x ", f2[j]);
   cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database environment
 try{

  db.rlogon("scott/tiger@postgresql"); // connect to the database

  db.direct_exec("set client_encoding='utf-8'"); // set the UTF-8 client encoding

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
