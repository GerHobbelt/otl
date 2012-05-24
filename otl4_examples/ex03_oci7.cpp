#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA7 // Compile OTL 4/OCI7
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void plsql(void)
// invoking PL/SQL block
{ 
 otl_stream o(5, // buffer size
              "begin "
              " :A<int,inout> := :A+1; "
              " :B<char[31],out> := :C<char[31],in>; "
              "end;",
                 // PL/SQL block
              db // connect object
             );

 o.set_commit(0); // set stream auto-commit off since 
                  // the block does not have any transactions
                  // to commit

 o<<1<<"Test String1"; // assigning :A = 1, :C = "Test String1"
 o<<2<<"Test String2"; // assigning :A = 2, :C = "Test String2"
 o<<3<<"Test String3"; // assigning :A = 3, :C = "Test String3"

 o.flush(); // executing PL/SQL block 3 times

 int a;
 char b[32];

 while(!o.eof()){ // not end-of-data
  o>>a>>b;
  cout<<"A="<<a<<", B="<<b<<endl;
 }

}


int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("scott/tiger"); // connect to Oracle

  plsql();

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
