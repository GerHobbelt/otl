#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 

 otl_long_string f2(70000); // define long string variable
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream o(1, // buffer size needs to be set to 1 for long strings
              "insert into test_tab values(:f1<int>,:f2<raw_long>)", 
                 // SQL statement
              db // connect object
             );


 for(int i=1;i<=20;++i){
  for(int j=0;j<50000;++j)
   f2[j]='*';
  f2[50000]='?';
  f2.set_len(50001);
  o<<i<<f2;
 }
}

void select(void)
{ 
 otl_long_string f2(70000); // define long string variable
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream i(1, // buffer size needs to be set to 1
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
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=mssql2008"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 image)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

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
