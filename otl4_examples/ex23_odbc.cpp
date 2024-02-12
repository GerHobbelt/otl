#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>
#define OTL_ODBC // Compile OTL 4/ODBC
#include <otlv4.h>

otl_connect db; // connect object

void stored_proc(void)
// invoking stored procedure
{ 
 otl_stream o(1, // buffer size should be equal to 1 in case of stored procedure call
              "{call my_proc("
              " :A<int,inout>, "
              " :B<char[31],out>, "
              " :C<char[31],in> "
              ")}",
                 // stored procedure call
              db // connect object
             );

 o.set_commit(0); // set stream auto-commit off since
                  // the stream does not generate transaction

 o<<1<<"Test String1"; // assigning :1 = 1, :3 = "Test String1"

 int a=0;
 char b[31];

 o>>a>>b;
 cout<<"A="<<a<<", B="<<b<<endl;

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{
  db.rlogon("uid=system;pwd=oracle;dsn=my_db"); // connect to data source MY_DB
  otl_cursor::direct_exec
   (
    db,
    "CREATE OR REPLACE PROCEDURE my_proc "
    "  (A IN OUT NUMBER, "
    "   B OUT VARCHAR2, "
    "   C IN VARCHAR2) "
    "IS "
    "BEGIN "
    "   A := A+1; "
    "   B := C; "
    "END;"
    );  // create stored procedure

  stored_proc(); // invoking stored procedure
 }
 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.code<<endl; // print out error code
  cerr<<p.var_info<<endl; // print out the variable that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
 }
 db.logoff(); // disconnect from the data source
 return 0;
} 
