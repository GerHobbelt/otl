#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
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

   db1.server_attach("myora_tns"); // Attach to a local Oracle server,
   // In case of a remote server, specify 
   // a TNS alias

  cout<<"Changing system's password to 'pussycat'"<<endl;
  db1.change_password("system","oracle","pussycat");
  cout<<"Connecting to system/pussycat..."<<endl;
  db2.rlogon("system/pussycat@myora_tns");
  cout<<"Connected"<<endl;
  cout<<"Changing the password back to 'tiger'"<<endl;
  db1.change_password("system","pussycat","oracle");
  cout<<"Connecting to system/oracle..."<<endl;
  db3.rlogon("system/oracle@myora_tns");
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
