#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

//#define OTL_ORA8 // Compile OTL 4.0/OCI8
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
// #define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(20, // make the buffer size larger than the actual 
                  // row set to inserted, so that the stream will not
                  // flush the buffer automatically
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );

 o.set_commit(0); // set stream's auto-commit to OFF.
 o.set_batch_error_mode(true); // set stream's batch error mode to ON.

 try{
  o<<1<<"Line1"; // Enter one row into the stream
  o<<1<<"Line1"; // Enter the same data into the stream
                 // and cause a "duplicate key" error.
  o<<2<<"Line2"; // Enter one row into the stream
  o<<3<<"Line3"; // Enter one row into the stream
  o<<4<<"Line4"; // Enter one row into the stream
  o<<2000<<"Line2000"; // Enter an out-of-range f1 into the stream
  o<<otl_null()<<"LineNull"; // Enter a NULL f1 into the stream
  o.flush();
 }catch(otl_exception& p){
   if(p.code==24381){
     // ORA-24831, error(s) in array DML
     otl_exception error;
     int total_errors=o.get_number_of_errors_in_batch();
     cout<<"TOTAL_ERRORS="<<total_errors<<endl;
     int dml_row_offset;
     for(int i=0;i<total_errors;++i){
       o.get_error(i,dml_row_offset,error);
       cout<<"I="<<i<<", ROW_OFFSET="<<dml_row_offset
           <<", CODE="<<error.code<<", MSG="<<error.msg
           <<endl;
     }
   }
 }

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
    "create table test_tab(f1 number not null, f2 varchar2(30), "
    "constraint check1 check(f1 between 1 and 1000))"
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
