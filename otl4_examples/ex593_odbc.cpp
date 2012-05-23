#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC_ENTERPRISEDB // Compile OTL 4.0/EnterpriseDB-ODBC
// Uncomment the #define below when used in Linux / Unix
//#define OTL_ODBC_UNIX
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab2 values(:f1<int>,:f2<timestamp>)", 
                 // SQL statement
              db // connect object
             );

 otl_datetime tm;

 for(int i=1;i<=10;++i){
  tm.year=2008;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  o<<i<<tm;
 }
}

void select(void)
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab2 where f2=:f2<timestamp>",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 otl_datetime tm,f2;

 tm.year=2008;
 tm.month=10;
 tm.day=19;
 tm.hour=23;
 tm.minute=12;
 tm.second=12;

 i<<tm; // assigning :f2 = tm
   // SELECT automatically executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second<<endl;
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
    "drop table test_tab2",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table
  db.commit();
  otl_cursor::direct_exec
   (
    db,
    "create table test_tab2(f1 int, f2 date)"
    );  // create table
  db.commit();
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
