#include <iostream>
using namespace std;

#include <stdio.h>

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__CYGWIN__)
#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC
#else 
#define OTL_ODBC
#endif
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
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
              "{call my_proc(:f1<int>)}", // SELECT statement via stored procedure
              db, // connect object
              otl_implicit_select // implicit SELECT statement
             ); 
   // create select stream
 
 otl_column_desc* desc;
 int desc_len;

 i<<4; // Before the child SELECT statement can be described,
       // the stored procedure needs to be executed.
       // Forcing the execution of the stored procedure by entering its input 
       // patameter.

 desc=i.describe_select(desc_len);

 for(int n=0;n<desc_len;++n){
  cout<<"========== COLUMN #"<<n+1<<" ==========="<<endl;
  cout<<"name="<<desc[n].name<<endl;
  cout<<"dbtype="<<desc[n].dbtype<<endl;
  cout<<"otl_var_dbtype="<<desc[n].otl_var_dbtype<<endl;
  cout<<"dbsize="<<desc[n].dbsize<<endl;
  cout<<"scale="<<desc[n].scale<<endl;
  cout<<"prec="<<desc[n].prec<<endl;
  cout<<"nullok="<<desc[n].nullok<<endl;
 }
 

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tigger@sybsql"); // connect to ODBC

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
  otl_cursor::direct_exec(db,"drop procedure my_proc",0);
  otl_cursor::direct_exec
   (
    db,
    "CREATE PROCEDURE my_proc "
    "  @F1 int "
    "AS "
    "SELECT * FROM test_tab "
    "WHERE f1>=@F1 AND f1<=@F1*2 "
    );  // create stored procedure

  insert(); // insert record into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.code<<endl; // print out error code
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
