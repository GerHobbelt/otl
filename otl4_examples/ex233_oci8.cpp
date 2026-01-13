#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

//#define OTL_ORA8I // Compile OTL 4.0/OCI8
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
#define OTL_ORA_MAP_STRINGS_TO_CHARZ
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(5, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[11]>)", 
                 // SQL statement
              db // connect object
             );
  o<<1<<"Name";
  o<<2<<"Name ";
  o<<3<<"Name  ";
}

void select()
{ 
 otl_stream i(5, // buffer size
              "select * from test_tab where f2=:f2<char[11]>",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 char f2[31];

 i<<"Name"; // assigning :f2 = 'Name'
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2='"<<f2<<"'"<<endl;
 
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
    "create table test_tab(f1 number, f2 varchar2(10))"
    );  // create table

  cout<<"========= table with a VARCHAR2 column ========"<<endl;
  insert(); // insert records into table
  select(); // select records from table

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 char(10))"
    );  // create table

  cout<<"========= table with a CHAR column ========"<<endl;
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
