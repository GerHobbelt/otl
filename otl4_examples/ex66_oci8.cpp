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
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );

 o.set_commit(0); // set stream's auto-commit to OFF.

 try{
  o<<1<<"Line1"; // Enter one row into the stream
  o.flush(); // flush the strem buffer, i.e. force
             // the stream to execute
  o<<1<<"Line1"; // Enter the same data into the stream
                 // and cause a "duplicate key" error.
  o.flush();
 }catch(otl_exception& p){
  if(p.code==1){
   // ORA-0001: ... duplicate key ...
    cout<<"STREAM ERROR STATE="<<o.get_error_state()<<endl;
   o.clean(1); // clean up the stream's buffer
               // and clean up thestream's internal
               // error flag as well. By doing this, 
               // it's possible to recover from 
               // a database error without closing
               // the stream. Remember, the number of 
               // situtation when it's possible is 
               // limited and the recover procedure should
               // be carefully designed.
  }else
   throw; // re-throw the exception to the outer catch block.
 }


 o<<2<<"Line2"; // Enter one more row of data after
                // recovering from the "duplicate key" 
                // error
 o.flush();

 db.commit(); // commit transaction

}

void select()
{ 
 otl_stream i(10, // buffer size
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 char f2[31];

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

  otl_cursor::direct_exec
   (
    db,
    "create unique index ind001 on test_tab(f1)"
    );  // create unique index 

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
