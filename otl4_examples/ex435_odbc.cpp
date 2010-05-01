#include <iostream>
using namespace std;

#include <stdio.h>
#define OTL_ODBC
//#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC
#define OTL_EXTENDED_EXCEPTION // Enable extended field in otl_exception
#define OTL_THROWS_ON_SQL_SUCCESS_WITH_INFO
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
  try{
    // Setting the "OTL throws on SQL_SUCCESS_WITH_INFO flag" to true
    db.set_throw_on_sql_success_with_info(true); 
    // Executing a T-SQL batch of INSERT statements. 
    otl_cursor::direct_exec
      (db,
       "SET NOCOUNT ON; " // NOCOUNT needs to be set to ON in order to
                          // work with OTL.
       "INSERT INTO test_tab VALUES(1,'Name1'); "
       "INSERT INTO test_tab VALUES(2,'Name2'); "
       "INSERT INTO test_tab VALUES(2,'*Name2'); "
       "INSERT INTO test_tab VALUES(3,'Name3') ",
       otl_exception::enabled
      );
  }catch(otl_exception& p){ // intercept OTL exceptions
    cerr<<p.msg<<endl; // print out error message
    cerr<<p.stm_text<<endl; // print out SQL that caused the error
    cerr<<p.sqlstate<<endl; // print out SQLSTATE message
    cerr<<p.var_info<<endl; // print out the variable that caused the error
    if(p.arr_len>0){ // checking if the extended fields were populated
      for(int j=0;j<p.arr_len;++j){
        cout<<"MSG["<<j<<"]="<<p.msg_arr[j]<<endl; // message array
        cout<<"SQLSTATE["<<j<<"]="<<p.sqlstate_arr[j]<<endl; // sqlstate array
        cout<<"CODE["<<j<<"]="<<p.code_arr[j]<<endl; // code array
      }
    }
  }
  // Setting the "throw flag" back to false
  db.set_throw_on_sql_success_with_info(false); 

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tigger@sybsql"); // connect to MS SQL Server

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int not null unique, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE info.
  cerr<<p.var_info<<endl; // print out the variable that caused the error
  if(p.arr_len>0){ // checking if the extended fields were populated
   for(int j=0;j<p.arr_len;++j){
    cout<<"MSG["<<j<<"]="<<p.msg_arr[j]<<endl; // message array
    cout<<"SQLSTATE["<<j<<"]="<<p.sqlstate_arr[j]<<endl; // sqlstate array
    cout<<"CODE["<<j<<"]="<<p.code_arr[j]<<endl; // code array
   }
  }
 }

 db.logoff(); // disconnect from MS SQL Server
 return 0;

}
