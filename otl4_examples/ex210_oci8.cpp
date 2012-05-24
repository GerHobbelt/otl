#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI
//#define OTL_ORA8I // Compile OTL 4.0/OCI
//#define OTL_ORA7 // Compile OTL 4.0/OCI
#define OTL_ORA9I // Compile OTL 4.0/OCI
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 

 otl_long_string f2(2000); // define long string variable
 db.set_max_long_size(2000); // set maximum long string size for connect object

 otl_stream o(1, // buffer size should be 1
              "insert into test_tab values(:f1<int>,:f2<raw_long>)", 
                 // SQL statement
              db // connect object
             );
  o.set_commit(0); // set stream's "auto-commit" to OFF
 
 for(int i=1;i<=20;++i){
  for(int j=0;j<1000;++j)
   f2[j]='*';
  f2[1000]='?';
  f2.set_len(1001);
  o<<i<<f2;
 }
 o.flush(); // flushing thes tream's buffer
 db.commit(); // committing transaction
}

void select(void)
{ 
 otl_long_string f2(2000); // define long string variable
 db.set_max_long_size(2000); // set maximum long string size for connect object

 otl_stream i(1, // buffer should be set to 1
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }
 
 i<<4<<4; // assigning :1 = 4, :2 = 4
   // SELECT automatically re-executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
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
    "create table test_tab(f1 int, f2 raw(2000))"
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
