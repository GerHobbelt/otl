#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__CYGWIN__)
#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC
#else 
#define OTL_ODBC
#endif
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

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db1.rlogon("sa/tigger@sybsql"); // connect to ODBC
  db2.rlogon("sa/tigger@sybsql"); // connect to ODBC
  db3.rlogon("sa/tigger@sybsql"); // connect to ODBC

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
    "create table test_tab(f1 int not null, f2 varchar(30), primary key(f1))"
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
