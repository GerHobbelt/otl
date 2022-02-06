#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(3, // stream buffer size in logical rows.
              "insert into test_tab "
              "values(:f1<int>,:f2<db2date>, "
              "       :f3<db2time>,:f4<timestamp>)", 
                 // INSERT statement
              db // connect object
             );

 otl_datetime tm1, tm2, tm3;

 for(int i=1;i<=10;++i){

  tm1.year=1998;
  tm1.month=10;
  tm1.day=19;
// time conponents should be set 0 in this 
// because it does not work otherwise for DB2 8.1, 
// and it works fine for earlier versions 
// of DB2 (6.x, 7.x)
  tm1.hour=0;
  tm1.minute=0;
  tm1.second=0;

// date conponents should be set 0 in this 
// because it does not work otherwise for DB2 8.1, 
// and it works fine for earlier versions 
// of DB2 (6.x, 7.x)
  tm2.year=0;
  tm2.month=0;
  tm2.day=0;

  tm2.hour=23;
  tm2.minute=12;
  tm2.second=12;

  tm3.year=1998;
  tm3.month=10;
  tm3.day=19;
  tm3.hour=23;
  tm3.minute=12;
  tm3.second=12;

  o<<i<<tm1<<tm2<<tm3;

 }
}

void select(void)
{ 
 otl_stream i(5, // stream buffer size in logical rows
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
 
 int f1=0;
 otl_datetime f2,f3,f4;

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3>>f4;
  cout<<"f1="<<f1;
  cout<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year;
  cout<<", f3="<<f3.hour<<":"<<f3.minute<<":"
      <<f3.second;
  cout<<", f4="<<f4.month<<"/"<<f4.day<<"/"
      <<f4.year<<" "<<f4.hour<<":"<<f4.minute<<":"
      <<f4.second;
  cout<<endl;
 }
 
}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2-CLI environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to DB2

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 date, f3 time, f4 timestamp)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

  db.logoff(); // disconnect from DB2

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 return 0;

}
