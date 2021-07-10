#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 

 otl_long_string f2(7000); // define long string variable
 db.set_max_long_size(7000); // set maximum long string size for connect object

 otl_stream 
   o(1, // buffer size needs to be set to 1
     "insert into test_tab values(:f1<int>,:f2<varchar_long>)", 
        // INSERT statement
      db // connect object
     );

 for(int i=1;i<=10;++i){
  for(int j=0;j<5000;++j)
   f2[j]='*';
  f2[5000]='?';
  f2.set_len(5001);
  o<<i<<f2;
 }
}

void select(void)
{ 
 otl_long_string f2(7000); // define long string variable
 db.set_max_long_size(7000); // set maximum long string size for connect object

 otl_stream i(1, // buffer size needs to be set to 1 in case of TEXT columns
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;


 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@sqlite");

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 text)"
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
