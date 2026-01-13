#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#define OTL_UNICODE // Enable Unicode OTL for DB2 CLI
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_long_unicode_string f2(70000); // define long unicode string variable
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream o(1, // buffer size has to be set to 1 for operations with CLOBs
              "insert into test_tab values(:f1<int>,:f2<varchar_long>)",
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=20;++i){
  for(int j=0;j<50000;++j)
    f2[j]=static_cast<unsigned short>('*');
  // first 128 characters of Unicode are ASCII
  f2[50000]=static_cast<unsigned short>('?'); 
  // first 128 characters of Unicode are ASCII
  f2.set_len(50001);
  o<<i<<f2;
 }

}

void select()
{ 
 otl_long_unicode_string f2(70000); // define long string variable
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream i(10,
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="
      <<static_cast<char>(f2[0]) // first 128 characters of Unicode are ASCII
      <<static_cast<char>(f2[f2.len()-1])<<", len="<<f2.len()<<endl;
 
}

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<static_cast<char>(f2[0])
      <<static_cast<char>(f2[f2.len()-1])<<", len="<<f2.len()<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database environment
 try{

  db.rlogon("scott/tiger@db2sql2"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 dbclob(1M))"
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

