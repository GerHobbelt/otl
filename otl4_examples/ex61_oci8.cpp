#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4/OCI8
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
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
  o<<static_cast<float>(i)<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1=0;
 char f2[31];

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("system/oracle@myora_tns"); // connect to Oracle

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 varchar2(30))"
    );  // create table

  insert(); // insert records into table
 
  db.logoff(); // disconnect from Oracle

  db.server_attach("myora_tns"); // attach to the local Oracle server
                      // In order to connect to a remote server,
                      // a TNS alias needs to be specified

  cout<<"Session begin ==> "<<1<<endl;

  db.session_begin("system","oracle");
     // begin session; this function is much faster
     // than rlogon() and should be used (see the Oracle
     // manuals for more detail) in high-speed processing
     // systems, possibly with thousands of users.
     // this technique can be used instead of traditional
     // connection pooling.

   cout<<"Session end ==> "<<1<<endl;
   db.session_end(); // end session

  for(int i=2;i<=100;++i){
   cout<<"Session begin ==> "<<i<<endl;
   db.session_reopen();
     // reopen session, previously opened by session_begin().
     // this function open a new session with the same username/password,
     // that were used by the previous session_begin() call. Session_reopen()
     // works 5-10% faster than session_begin(), because the username and
     // password attributes of the OCI8 connect structure have been already
     // set.

   select(); // select records from table

   cout<<"Session end ==> "<<i<<endl;
   db.session_end(); // end session

  }


  db.server_detach(); // detach from the Oracle server

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // make sure that the program gets disconnected from Oracle

 return 0;

}
