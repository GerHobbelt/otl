#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI8
#define OTL_ODBC // Compile OTL 4.0/ODBC
// In case when <OTL_ORA8,OTL_ODBC> or <OTL_ORA7,OTL_ODBC> pair is
// defined, the OTL header file generates namespaces oracle:: and odbc::
// respectively, in order to make the names of the otl_xxx classes
// unique. 

#include <otlv4.h> // include the OTL 4.0 header file

const int long_text_max=70000; // maximum length of long strings.

odbc::otl_connect db1; // connect object
oracle::otl_connect db2; // connect object

void insert1(void)
// insert rows into ODBC table
{ 

 otl_long_string f2(long_text_max); // define long string variable
 db1.set_max_long_size(long_text_max); // set maximum long string size for connect object

 odbc::otl_stream o(1, // buffer size needs to be set to 1 for long strings
                   "insert into test_tab values(:f1<int>,:f2<varchar_long>)", 
                      // SQL statement
                    db1 // connect object
                   );

 for(int i=1;i<=20;++i){
  for(int j=0;j<50000;++j)
   f2[j]='*';
  f2[50000]='?';
  f2.set_len(50001);
  o<<i<<f2;
 }
}

void insert2()
// insert rows into OCI table
{ 

 otl_long_string f2(long_text_max); // define long string variable
 db2.set_max_long_size(long_text_max); // set maximum long string size for connect object
 
 int f1;

// INSERT statement for inserting rows into OCI table
 oracle::otl_stream 
  o(1, // buffer size has to be set to 1 for operations with LOBs
    "insert into test_tab2 values(:f1<int>,empty_clob()) "
    "returning f2 into :f2<clob> ",
        // SQL statement
     db2 // connect object
    );

// SELECT statement to read rows from OCI table
 odbc::otl_stream 
  i(1, // buffer size needs to be set to 1
    "select * from test_tab", // SELECT statement
    db1 // connect object
   ); 

 while(!i.eof()){
  i>>f1>>f2; // Reading one row from ODBC
  o<<f1<<f2; // Inserting one row into OCI
 }

}

void select2()
// Select rows from OCI table
{ 
 otl_long_string f2(long_text_max); // define long string variable
 db2.set_max_long_size(long_text_max); // set maximum long string size for connect object

 oracle::otl_stream 
  i(10, // buffer size. To read CLOBs, it can be set to a size greater than 1
    "select * from test_tab2 where f1>=:f<int> and f1<=:f*2",
        // SELECT statement
    db2 // connect object
   ); 
   // create select stream
 
 float f1;

 i<<8; // assigning :f = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

}

int main()
{
 oracle::otl_connect::otl_initialize(); // initialize OCI environment
 odbc::otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db1.rlogon("UID=scott;PWD=tiger;DSN=my_db"); // connect to ODBC
  db2.rlogon("scott/tiger"); // connect to OCI

  odbc::otl_cursor::direct_exec
   (
    db1,
    "drop table test_tab",
    odbc::otl_exception::disabled // disable OTL exceptions
   ); // drop table

  odbc::otl_cursor::direct_exec
   (
    db1,
    "create table test_tab(f1 number, f2 long)"
    );  // create table

  oracle::otl_cursor::direct_exec
   (
    db2,
    "drop table test_tab2",
    oracle::otl_exception::disabled // disable OTL exceptions
   ); // drop table

  oracle::otl_cursor::direct_exec
   (
    db2,
    "create table test_tab2(f1 number, f2 clob)"
    );  // create table

  insert1(); // insert records into MSSQL table
  insert2(); // Read the ODBC table and insert records 
             // into OCI table
  select2(); // select records from the Oracle table.

 }

 catch(odbc::otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQlSTATE
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 catch(oracle::otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db1.logoff(); // disconnect from ODBC
 db2.logoff(); // disconnect from OCI

 return 0;

}
