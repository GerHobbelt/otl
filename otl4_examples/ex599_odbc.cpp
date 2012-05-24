#include <iostream>
#include <string>

#define OTL_ODBC_ENTERPRISEDB // Compile OTL 4.0/EnterpriseDB-ODBC
// Uncomment the #define below when used in Linux / Unix
//#define OTL_ODBC_UNIX
#define OTL_STL // Turn on STL features and otl_value<T>
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif
#include <otlv4.h> // include the OTL 4.0 header file

using namespace std;

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab "
              "values(:f1<int>,:f2<char[31]>,:f3<timestamp>)", 
                 // SQL statement
              db // connect object
             );

 otl_value<string> f2; // otl_value container of STL string
 otl_value<otl_datetime> f3; // container of otl_datetime


 for(int i=1;i<=100;++i){

  if(i%2==0)
   f2="NameXXX";
  else
   f2=otl_null(); // Assign otl_null() to f2

  if(i%3==0){
   // Assign a value to f3 via the .v field directly
   f3.v.year=2001;
   f3.v.month=1;
   f3.v.day=1;
   f3.v.hour=13;
   f3.v.minute=10;
   f3.v.second=5;
   f3.set_non_null(); // Set f3 as a "non-NULL"
  }else
   f3.set_null(); // Set f3 as null via .set_null() function

  o<<i<<f2<<f3;

 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream

 int f1; 
 otl_value<string> f2;
 otl_value<otl_datetime> f3;


 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3;
  cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<f3<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3;
  cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<f3<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@edbsql"); // connect to EDB

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 varchar2(30), f3 date)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from EDB

 return 0;

}
