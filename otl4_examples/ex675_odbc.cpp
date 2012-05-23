#include <iostream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OTL_ODBC_MULTI_MODE // Compile OTL 4.0/ODBC multi-mode
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(const int buf_size)
// insert rows into table
{ 
 otl_stream o(buf_size,
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
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
  o<<i<<tmp;
 }
}

void update(const int af1, const int buf_size)
// insert rows into table
{ 
 otl_stream o(buf_size,
              "UPDATE test_tab "
              "   SET f2=:f2<char[31]> "
              " WHERE f1=:f1<int>", 
                 // UPDATE statement
              db // connect object
             );
 o.set_commit(0); // turn OFF the stream's "auto-commit"

 o<<"Name changed"<<af1;
 o<<otl_null()<<af1+1; // set f2 to NULL

}

void select(const int af1, const int buf_size)
{ 
 otl_stream i(buf_size,
              "select * from test_tab "
              "where f1>=:f11<int> "
              "  and f1<=:f12<int>*2",
                // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 char f2[31];

 i<<af1<<af1; // :f11 = af1, :f12 = af1
 while(!i.eof()){ // while not end-of-data
  i>>f1;
  cout<<"f1="<<f1<<", f2=";
  i>>f2;
  if(i.is_null())
   cout<<"NULL";
  else
   cout<<f2;
  cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

// ===================== Sybase via ODBC ======================================

  db.set_connection_mode(OTL_DEFAULT_ODBC_CONNECT);
  db.rlogon("scott/tigger@sybsql");

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

  db.commit();

  cout<<"============== Sybase via ODBC ====================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== TimesTen via ODBC ======================================

  db.set_connection_mode(OTL_TIMESTEN_ODBC_CONNECT);
  db.rlogon("scott/tiger@TT_tt1121_32");

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

  db.commit();

  cout<<"============== TimesTen via TimesTen ODBC driver ==================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,1); // select records from the table, 
               // stream buffer size should be set to 1 
               // when connected to TimesTen via TimesTen ODBC driver
  cout<<"===================================================================="<<endl;

  db.logoff(); // disconnect from ODBC


// ===================== MS SQL 2008 via ODBC ==============================

  db.set_connection_mode(OTL_MSSQL_2008_ODBC_CONNECT);
  db.rlogon("scott/tiger@mssql2008");

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

  db.commit();

  cout<<"=========== MS SQL 2008 via ODBC ==================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== PostgreSQL via ODBC ==============================

  db.set_connection_mode(OTL_POSTGRESQL_ODBC_CONNECT);
  db.rlogon("scott/tiger@postgresql");

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

  db.commit();

  cout<<"=========== PostgreSQL via ODBC ==================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== EntipriseDB via ODBC ==============================

  db.set_connection_mode(OTL_ENTERPRISE_DB_ODBC_CONNECT);
  db.rlogon("scott/tiger@edbsql");

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

  db.commit();

  cout<<"=========== Enterprise DB via ODBC ================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== MySQL via ODBC ==============================

  db.set_connection_mode(OTL_MYODBC35_ODBC_CONNECT);
  db.rlogon("scott/tiger@mysql35");

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

  db.commit();

  cout<<"=========== MySQL via MyODBC 3.5 ==================="<<endl;
  insert(1); // insert records into the table, stream buffer size should be set to 1
  update(10,1); // update records in the table, stream buffer size should be set to 1
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

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

