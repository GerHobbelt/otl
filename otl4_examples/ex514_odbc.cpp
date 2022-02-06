#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_FREETDS_ODBC_WORKAROUNDS
#define OTL_ODBC_MSSQL_2005 // Compile OTL 4/ODBC, MS SQL 2005
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
//#define OTL_ODBC_UNIX // Compile OTL 4 / ODBC. Uncomment this when used in Linux / Unix
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(12345678900000000+convert(numeric,:f1<int>),:f11<int>,:f2<char[31]>, GETDATE())", 
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
  o<<i<<i<<tmp;
 }
}

void select()
{ 
 otl_stream i;

 i.set_all_column_types(otl_all_num2str | otl_all_date2str); 
   // map all numeric columns to strings as well as
   // map all date columns to strings

 i.open(50, // buffer size
        "select * from test_tab "
        "where f1>=12345678900000000+convert(numeric,:f11<int>) "
        "  and f1<=12345678900000000+convert(numeric,:f12<int>)*2",
           // SELECT statement
        db // connect object
       ); 
   // create select stream
 
 char f1[40];
 char f11[40];
 char f2[31];
 char f3[40];

 i<<8<<8; 
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f11>>f2>>f3;
  cout<<"f1="<<f1<<", f11="<<f11<<", f2="<<f2<<", f3="<<f3<<endl;
 }

 i<<4<<4;
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f11>>f2>>f3;
  cout<<"f1="<<f1<<", f11="<<f11<<", f2="<<f2<<", f3="<<f3<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@freetds_mssql"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 numeric, f11 int, f2 varchar(30), f3 datetime)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
