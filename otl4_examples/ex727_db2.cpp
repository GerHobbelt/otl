#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <stdio.h>
using namespace std;

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#include <otlv4.h> // include the OTL 4.0 header file


otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(10, // stream buffer size in logical rows.
              "insert into test_tab "
              "values(:f1<int>,:f2<char[31]>)",
                 // INSERT statement
              db // connect object
             );
  o<<1<<"Line1";
  o<<2<<"Line21";
  o.flush();
}

void merge(void)
// merge rows into table
{ 
 otl_stream o(10, // stream buffer size in logical rows.
              "merge into test_tab d "
              "using (select cast(:f1<int> as int) f1, "
              "              cast(:f2<char[31]> as varchar(30)) f2 "
              "       from sysibm.sysdummy1) s "
              "on (s.f1=d.f1) "
              "when matched then update set d.f2=s.f2||'--'||cast(s.f1 as char(10)) "
              "when not matched then insert (d.f1, d.f2) values(s.f1,s.f2)",
                 // MERGE statement
              db // connect object
             );
  o<<2<<"Line22";
  o<<3<<"Line3";
  o.flush();
}


void select(void)
{ 
 otl_stream i(5, // stream buffer size in logical rows
              "select * from test_tab ",
                 // SELECT statement
              db // connect object
             ); 
// SELECT output columns
 int f1=0;
 char f2[31];

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }
 
}


int main()
{
 otl_connect::otl_initialize(); // initialize DB API environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab (f1 int, f2 varchar(30))"
    );  // create table

  db<<"create unique index ind001 on test_tab(f1)";
     // create unique index

  insert(); // insert records into table
  merge(); // merge records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
