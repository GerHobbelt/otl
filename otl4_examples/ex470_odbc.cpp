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

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(1, // buffer size
              "insert into test_tab (f2) "
               "OUTPUT INSERTED.f1, INSERTED.f2 "
               "values(:f2<char[31],in>)", 
                 // SQL statement
              db, // connect object
              otl_implicit_select // the statement returns an implicit result set
             );
 o.set_commit(0); // set stream auto-commit to OFF
 char f2_in[32];
 int f1=0;
 char f2[32];

 for(int i=1;i<=10;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
   sprintf_s(f2_in,sizeof(f2_in),"Name%d",i);
#else
   sprintf(f2_in,"Name%d",i);
#endif
#else
   sprintf(f2_in,"Name%d",i);
#endif
   o<<f2_in; // write input variable :f2
   for(auto& it : o){ // while not end-of-data
 // while not end-of-data
     it>>f1>>f2; // fetch the columns from the OUTPUT clause
     cout<<"f1="<<f1<<", f2="<<f2<<endl;
   
}
 }
 db.commit(); // commit transaction

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int not null identity, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
