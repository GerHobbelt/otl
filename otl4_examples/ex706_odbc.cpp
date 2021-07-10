#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
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

void insert()
// insert rows into table
{ 
 db.set_max_long_size(80000); // set maximum long string size for connect object
 otl_stream o(10, // buffer size 
              "insert into test_tab values(:f1<int>,:f2<varchar_long>) ",
                 // SQL statement
              db // connect object
             );
 char tmp[80001];

 for(int i=1;i<=20;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
   sprintf_s(reinterpret_cast<char*>(tmp),sizeof(tmp),"<TAG>MyXML%d</TAG>",i);
#else
  sprintf(reinterpret_cast<char*>(tmp),"<TAG>MyXML%d</TAG>",i);
#endif
#else
  sprintf(reinterpret_cast<char*>(tmp),"<TAG>MyXML%d</TAG>",i);
#endif
  int len=static_cast<int>(strlen(tmp));
  otl_long_string f2(tmp,len,len); // define long string variable
  o<<i<<f2;
 }

}

void select()
{ 
 otl_long_string f2(80000); // define long string variable
 db.set_max_long_size(80000); // set maximum long string size for connect object

 otl_stream i(10, // buffer size
              "select f1, f2 "
              "from test_tab "
              "where f1>=:f11<int> "
              " and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8 
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<static_cast<unsigned char*>(&f2[0])<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@postgresql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 xml)"
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
