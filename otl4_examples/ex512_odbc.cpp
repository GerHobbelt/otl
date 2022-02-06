#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_FREETDS_ODBC_WORKAROUNDS
#define OTL_ODBC_MSSQL_2005 // Compile OTL 4/ODBC, MS SQL 2005
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
//#define OTL_ODBC_UNIX // Compile OTL 4 / ODBC. Uncomment this when used in Linux / Unix
#define OTL_EXTENDED_EXCEPTION // Enable extended field in otl_exception
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>", 
                 // SQL statement
              db // connect object
             );

 o<<100<<"Row 100";

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@freetds_mssql"); // connect to MS SQL Server

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30))"
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
