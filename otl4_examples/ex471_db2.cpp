#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>
#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#include <otlv4.h>

otl_connect db; // connect object

void stored_proc(void)
// invoking stored procedure
{ 
 otl_stream o(1, // buffer size should be equal to 1 in case of stored procedure call
              "{ :rc<int,out> = call my_proc("
              " :A<int,inout>, "
              " :B<char[31],out>, "
              " :C<char[31],in> "
              ")}",
                 // stored procedure call
              db // connect object
             );

 o.set_commit(0); // set stream auto-commit off since
                  // the stream does not generate transaction

 o<<1<<"Test String1"; // assigning :A = 1, :C = "Test String1"

 int rc=0;
 int a=0;
 char b[31];

 o>>rc>>a>>b;
 cout<<"RC="<<rc<<", A="<<a<<", B="<<b<<endl;

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2 CLI environment
 try{
  db.rlogon("scott/tiger@db2sql"); // connect to DB2
  otl_cursor::direct_exec
   (db,"DROP PROCEDURE my_proc",0); // drop procedure and ignore any errors
  otl_cursor::direct_exec
   (db,
    "create procedure my_proc "
     "(inout a integer, "
     " out b varchar(60), "
     " inout c varchar(60)) "
     "language SQL "
     "begin "
     "  set a = a + 1; "
     "  set b = c; "
     "  return 100; "
     "end "
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
