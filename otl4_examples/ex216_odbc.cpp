#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OTL_ODBC // Compile OTL 4.0 / ODBC Firebird
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#define OTL_ADD_NULL_TERMINATOR_TO_STRING_SIZE
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(1, // buffer size should be == 1 always on INSERT
              "insert into test_tab values(:f1<int>,:f2<char[2]>)", 
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=100;++i){
  o<<i<<"AB";
 }
}

void update(const int af1)
// insert rows into table
{ 
 otl_stream o(1, // buffer size should be == 1 always on UPDATE
              "UPDATE test_tab "
              "   SET f2=:f2<char[31]> "
              " WHERE f1=:f1<int>", 
                 // UPDATE statement
              db // connect object
             );

 o<<"CD"<<af1;
 o<<otl_null()<<af1+1; // set f2 to NULL

}

void select(const int af1)
{ 
 otl_stream i(50, // buffer size may be > 1
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2", 
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 char f2[31];

 i<<af1<<af1; // Writing input values into the stream
 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1;
  cout<<"f1="<<f1<<", f2=";
  it>>f2;
  if(it.is_null())
   cout<<"NULL";
  else
   cout<<f2;
  cout<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("sysdba/tiger@firebirdsql"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(2))"
    );  // create table

  db.commit(); // committing the create table statement to to the database

  insert(); // insert records into the table
  update(10); // update records in the table
  select(8); // select records from the table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
