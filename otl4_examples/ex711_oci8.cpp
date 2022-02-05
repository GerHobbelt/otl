#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8 // Compile OTL 4.0/OCI8
// uncomment the version of the OCI you're using
// #define OTL_ORA8I // Compile OTL 4.0/OCI8i
// #define OTL_ORA9I // Compile OTL 4.0/OCI9i
// #define OTL_ORA10G // Compile OTL 4.0/OCI10g
// #define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10gR2
// #define OTL_ORA11G // Compile OTL 4.0/OCI11g
// #define OTL_ORA11G_R2 // Compile OTL 4.0/OCI11gR2
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];
 int i;

 for(i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<i<<tmp; //<<endr; // OTL stream manipulator "endr" checks 
                   // the "end of row" condition. As time goes on,
                   // the table layout may change, and the end-of-row check
                   // may throw an exception if the end-of-row condition
                   // fails.
 }
 o.flush(); // flush the stream dirty buffer
 // example of a faied end-of-row condition
 try{
  o<<i;
  // input for bind variable :f2 is missing
  o<<endr; // this will throw an exception
 }catch(otl_exception& p){
   o.clean(1); // reset the stream state
  cerr<<"ERROR CODE="<<p.msg<<endl; // print out error message
  cerr<<"ERROR SQL STM="<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<"ERROR VAR INFO="<<p.var_info<<endl; // print out the variable that caused the error
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
 
 int f1=0;
 char f2[31];

 i<<8; // assigning :f = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>endr; // endr checks the end-of-row condition. As times goes on,
                   // The output column list on the SELECT statement may change,
                   // and the endr check may an exception.
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer
 try{
// example of a failed end-of-row check
   while(!i.eof()){ // while not end-of-data
    i>>f1;
    // reading of the f2 output column is missing
    i>>endr; // this will throw an exception
    cout<<"f1="<<f1<<", f2="<<f2<<endl;
   }
 }catch(otl_exception& p){
  cerr<<"ERROR CODE (SELECT) ="<<p.msg<<endl; // print out error message
  cerr<<"ERROR SQL STM (SELECT) ="<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<"ERROR VAR INFO (SELECT) ="<<p.var_info<<endl; // print out the variable that caused the error
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
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
