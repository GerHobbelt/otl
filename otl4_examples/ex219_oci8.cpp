#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI8i
// #define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db1, db2, db3; // connect objects

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db1.server_attach(); // Attach to a local Oracle server,
                      // In case of a remote server, specify 
                      // a TNS alias

  cout<<"Changing scott's password to 'pussycat'"<<endl;
  db1.change_password("scott","tiger","pussycat");
  cout<<"Connecting to scott/pussycat..."<<endl;
  db2.rlogon("scott/pussycat");
  cout<<"Connected"<<endl;
  cout<<"Changing the password back to 'tiger'"<<endl;
  db1.change_password("scott","pussycat","tiger");
  cout<<"Connecting to scott/tiger..."<<endl;
  db3.rlogon("scott/tiger");
  cout<<"Connected"<<endl;
 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db1.server_detach();
 db2.logoff();
 db3.logoff();
  
 return 0;

}
