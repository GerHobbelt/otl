#include <iostream>
using namespace std;
#include <stdio.h>

unsigned int my_trace_level=
   0x1 | // 1st level of tracing
   0x2 | // 2nd level of tracing
   0x4 | // 3rd level of tracing
   0x8 | // 4th level of tracing
   0x10; // 5th level of tracing
// each level of tracing is represented by its own bit, 
// so levels of tracing can be combined in an arbitrary order.

#define OTL_TRACE_LEVEL my_trace_level 
   // enables OTL tracing, and uses my_trace_level as a trace control variable.

#define OTL_TRACE_STREAM cerr 
   // directs all OTL tracing to cerr

#define OTL_TRACE_LINE_PREFIX "MY OTL TRACE ==> " 
   // redefines the default OTL trace line prefix. This #define is optional

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
// #define OTL_ORA8I // Compile OTL 4.0/OCI8i
// #define OTL_ORA8I // Compile OTL 4.0/OCI8
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(int i=1;i<=23;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<i<<tmp;
 }
}

void select()
{ 
 otl_stream i(5, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:ff<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1;
 char f2[31];

 i<<8<<8; // assigning :f = 8; :ff = 8
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

  db.rlogon("scott/tiger"); // connect to the database

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

 db.logoff(); // disconnect from the database

 return 0;

}
