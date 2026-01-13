#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object for the first connection which will be
                // set to "auto-commit"
otl_connect db2; // connect object for the second connection which will be
                 // used for selecting "auto-commited" rows

void insert()
// insert rows into table
{ 
// otl_nocommit_stream is more logical to use in this case because transactions
// commit anyway.
 otl_nocommit_stream o
    (50, // buffer size
     "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
        // SQL statement
     db // connect object
    );
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

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f1<int> and f1<=:f2<int>*2",
                 // SELECT statement
              db2 // connect object
             ); 
   // create select stream
 
 float f1=0;
 char f2[31];

 i<<8<<8; // assigning :f1 = 8, :f2 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 
}

 i<<4<<4; // assigning :f1 = 4, :f2 = 4
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

  db.rlogon("scott/tiger@mssql2008",1); // connect to ODBC in the auto-commit mode.
  db2.rlogon("scott/tiger@mssql2008"); // connect to ODBC


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
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE 
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC
 db2.logoff(); // disconnect from ODBC

 return 0;

}
