#include <iostream>
using namespace std;
#include <stdio.h>

//#define OTL_ORA8I // Compile OTL 4.0/OCI8
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
// #define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(10, // make the buffer size larger than the actual 
                  // row set to inserted, so that the stream will not
                  // flush the buffer automatically
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );

 o.set_commit(0); // set stream's auto-commit to OFF.

 long total_rpc=0; // total "rows processed count"
 long rpc=0; // rows successfully processed in one flush() call
 int iters=0; // number of rows to be bypassed

 try{
  o<<1<<"Line1"; // Enter one row into the stream
  o<<1<<"Line1"; // Enter the same data into the stream
                 // and cause a "duplicate key" error.
  o<<2<<"Line2"; // Enter one row into the stream
  o<<3<<"Line3"; // Enter one row into the stream
  o<<4<<"Line4"; // Enter one row into the stream
  o<<4<<"Line4"; // Enter the same data into the stream
                 // and cause a "duplicate key" error.
  o.flush();
 }catch(otl_exception& p){
  if(p.code==1){
   // ORA-0001: ...duplicate key...
    ++iters;
    rpc=o.get_rpc();
    total_rpc=rpc;
    do{
      try{
        cout<<"TOTAL_RPC="<<total_rpc<<", RPC="<<rpc<<endl;
        o.flush(total_rpc+iters, // bypass the duplicate row and start
                                 // with the rows after that
                true // force buffer flushing regardless
               );
        rpc=0;
      }catch(otl_exception& p2){
        if(p2.code==1){
          // ORA-0001: ... duplicate key ...
          ++iters;
          rpc=o.get_rpc();
          total_rpc+=rpc;
        }else
          throw;
      }
    }while(rpc>0);
  }else
   throw; // re-throw the exception to the outer catch block.
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
 
 int f1;
 char f2[31];

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
