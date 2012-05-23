#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC_ALTERNATE_RPC
#if !defined(_WIN32) && !defined(_WIN64)
#define OTL_ODBC
#else 
#define OTL_ODBC_POSTGRESQL // required with PG ODBC on Windows
#endif

// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file


otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // PostgreSQL 8.1 and higher, the buffer can be > 1
              "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<tmp<<i;
 }
}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=postgresql"); // connect to ODBC

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
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table
  db.commit();
  insert(); // insert records into table

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
