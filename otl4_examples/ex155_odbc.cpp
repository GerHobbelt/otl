#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db1, db2, db3; // connect objects

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
                 // SQL statement
              db1 // connect object
             );
 o.set_commit(0); // set stream's auto-commit to OFF

 char tmp[32];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<static_cast<float>(i)<<tmp;
 }
 o.set_flush();
 // At this point, rows are posted but not yet committed to the database
}

void select(otl_connect& db)
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:ff<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1=0;
 char f2[31];

 i<<4<<4; // assigning :f = 4, :ff = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db1.rlogon("UID=scott;PWD=tiger;DSN=mssql2008"); // connect to ODBC

  db2.rlogon("UID=scott;PWD=tiger;DSN=mssql2008"); // connect to ODBC
  db3.rlogon("UID=scott;PWD=tiger;DSN=mssql2008"); // connect to ODBC

  db2.set_transaction_isolation_level(otl_tran_read_uncommitted);
  db3.set_transaction_isolation_level(otl_tran_read_committed);

  otl_cursor::direct_exec
   (
    db1,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db1,
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  db1.commit(); // committing the DDLs to the database
  
  insert(); // insert records into table

  cout<<"=======> Reading uncommitted data..."<<endl; 
  select(db2); // select records from table

  db1.commit(); // committing the inserted rows to the database

  cout<<"=======> Reading committed data..."<<endl; 
  select(db3); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db1.logoff(); // disconnect from ODBC
 db2.logoff(); // disconnect from ODBC
 db3.logoff(); // disconnect from ODBC


 return 0;

}
