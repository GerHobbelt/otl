#include <iostream>
using namespace std;

#include <windows.h>
#include <stdio.h>

#define OTL_ORA10G_R2
//#include "REV/otlv40174.h"
#include "otlv4.h"

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
  sprintf(tmp,"Name%d",i);
  o<<(float)i<<tmp;
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
 
 float f1;
 char f2[31];

 i<<8; // assigning :f = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment

 try{

  db.rlogon("scott/tiger"); // connect to Oracle

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

 }
 catch(otl_exception& p){ // intercept OTL exceptions
   cerr << "(init) Exception code: " << p.code << endl;
   cerr << "(init) Exception  msg: " << p.msg << endl;
   cerr << "(init) Exception stmt: " << p.stm_text << endl;
   cerr << "(init) Exception info: " << p.var_info << endl;
   return 0;
 }

 bool connected=true;
 bool done=false;
 while(!done) {

   /* Try to execute queries */
   try{

     /* Execute queries. Code here will allow execution on 
        a non-connected resource */
     select(); // select records from table

     /* Shutdown oracle manually so the issue is triggered.
        For example: /etc/init.d/oracle-xe stop */
     std::cout << "Please, shutdown Oracle now..." << std::endl;

     char buf[100];
     cin>>buf;
     Sleep(2000);
   }
   catch(otl_exception& p){ // intercept OTL exceptions for select operations
     /* with OTL 4.0.210 the exception is:
          (select) Exception code: 3440
          (select) Exception  msg: 123456789
          (select) Exception stmt: 
          (select) Exception info: 

        but OTL 4.0.174 will throw:
          (select) Exception code: 24324
          (select) Exception  msg: ORA-24324: service handle not initialized
          (select) Exception stmt: select * from test_tab where f1>=:f      and f1<=:f*2
          (select) Exception info: 
     */
     cerr << "(select) Exception code: " << p.code << endl;
     cerr << "(select) Exception  msg: " << p.msg << endl;
     cerr << "(select) Exception stmt: " << p.stm_text << endl;
     cerr << "(select) Exception info: " << p.var_info << endl;
     if(connected) {
         db.logoff();
         connected=false; /* try to logon again */
     }
   }
   
   /* Reconnect if resource was disconnect for any reason */
   try{
     if(!connected) {
       db.rlogon("scott/tiger");
       connected=true;
     }
   }
   catch(otl_exception& p){ // intercept OTL exceptions for rlogon
     cerr << "(connect) Exception code: " << p.code << endl;
     cerr << "(connect) Exception  msg: " << p.msg << endl;
     cerr << "(connect) Exception stmt: " << p.stm_text << endl;
     cerr << "(connect) Exception info: " << p.var_info << endl;
     if(connected) {
         db.logoff();
         connected=false; /* try to logon again */
     }
   }
  
   /* If Oracle is unreacheable in the next iteration
      we will execute some queries on a disconnected db resource */

 } /* while(1) */

 return 0;

}
