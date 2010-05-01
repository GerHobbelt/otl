// One of the following two #define's enables ODBC / DB2 CLI 
// Unicode SQLxxxW functions
#define UNICODE
#define _UNICODE

#include <iostream>
using namespace std;

#include <stdio.h>
#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#define OTL_UNICODE // Enable Unicode OTL for ODBC

// Enable OTL Unicode rlogon and otl_exception for ODBC
#define OTL_UNICODE_EXCEPTION_AND_RLOGON 

#if defined(__GNUC__)
#define OTL_UNICODE_CHAR_TYPE SQLWCHAR
#else
#define OTL_UNICODE_CHAR_TYPE wchar_t
#endif

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{
// Special Unicode rlogon function
  db.rlogon(L"scott",
            L"tiger",
            L"mssqlxxx"
           ); // connect to the database

 }

 catch(otl_exception& p){ // intercept OTL exceptions
#if defined(__GNUC__)
  OTL_UNICODE_CHAR_TYPE* c=p.msg;
  while(*c){
    cerr<<static_cast<char>(*c);
    ++c;
  }
  cerr<<endl;
  c=p.sqlstate;
  while(*c){
    cerr<<(char)*c;
    ++c;
  }
  cerr<<endl;
#else
  wcerr<<p.msg<<endl; // print out error message
  wcerr<<p.sqlstate<<endl; // print out error sqlstate
  wcerr<<p.stm_text<<endl; // print out SQL that caused the error
  wcerr<<p.var_info<<endl; // print out the variable that caused the error
#endif
 }

 db.logoff(); // disconnect from the database

 return 0;

}
