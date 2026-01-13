#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

//#define OTL_ORA8I // Compile OTL 4.0/OCI8
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
#define OTL_ORA_DECLARE_COMMON_READ_STREAM_INTERFACE
#include <otlv4.h> // include the OTL 4.0 header file

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
  o<<i<<tmp;
 }
}

void fetch(otl_read_stream_interface& s)
{
  cout<<"===================="<<endl;
  int f1=0;
  char f2[31];
  while(!s.eof()){ // while not end-of-data
    s>>f1>>f2;
    cout<<"f1="<<f1<<", f2="<<f2<<endl;
  
}
}


void select()
{ 
 otl_stream i(1, // buffer size
              "begin "
              " open :cur<refcur,out[50]> for "
     // :cur is a bind variable name, refcur -- its type, 
     // out -- output parameter, 50 -- the buffer size when this
     // reference cursor will be attached to otl_refcur_stream
              "  select * "
              "  from test_tab "
              "  where f1>=:f<int,in> and f1<=:f*2; "
              "end;", // PL/SQL block returns a referenced cursor
              db // connect object
             ); 
   // create select stream with referenced cursor

  i.set_commit(0); // set stream "auto-commit" to OFF.
 
 otl_refcur_stream s; // reference cursor stream for reading rows.

 i<<8; // assigning :f = 8
 i>>s; // initializing the refrence cursor stream with the output 
       // reference cursor.
 fetch(s);
 s.close(); // closing the reference cursor

 i<<4; // assigning :f = 4
 i>>s;

 otl_stream i2(50, // buffer size
              "select f1, f2 "
              "from test_tab "
              "where f1>=:f<int> and f1<=:f*2", // straight SELECT
              db // connect object
             ); 

  i2<<4; // :f = 4
  fetch(i2);

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
