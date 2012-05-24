#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
// The following #define is required with MyODBC 3.51.11 and higher
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 

 otl_long_string f2(700); // define long string variable
 otl_stream o(1, // buffer size still has to be set to 1 on INSERT
              "insert into test_tab values "
              "(:f1<int>,:f2<raw[700]>), "
              "(:f12<int>,:f22<raw[700]>), "
              "(:f13<int>,:f23<raw[700]>), "
              "(:f14<int>,:f24<raw[700]>), "
              "(:f15<int>,:f25<raw[700]>) ",
// INSERT statement. Multiple sets of values can be used
// to work around the lack of the bulk interface
              db // connect object
             );
 for(int i=1;i<=20;++i){
  for(int j=0;j<699;++j)
   f2[j]='*';
  f2[699]='?';
  f2.set_len(700);
  o<<i<<f2;
 }

}

void select()
{ 
 otl_long_string f2(700); // define long string variable
 otl_stream i(10, // buffer size
              "select f1, "
// MyODBC does not return SQL_VARBINARY, so the output column type
// has to be overriden explicitly
              "       f2 :#2<raw[700]> "
              "from test_tab "
              "where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@mysql35"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varbinary(700)) type=innoDB"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.sqlstate<<endl; // print out SQLSTATE 
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
