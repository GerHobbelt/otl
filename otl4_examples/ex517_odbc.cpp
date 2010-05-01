#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_FREETDS_ODBC_WORKAROUNDS
#define OTL_ODBC_MSSQL_2005 // Compile OTL 4/ODBC, MS SQL 2005
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
//#define OTL_ODBC_UNIX // Compile OTL 4 / ODBC. Uncomment this when used in Linux / Unix
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect main_db; // main connect object
otl_connect secondary_db; // secondary connection object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
                 // SQL statement
              main_db // connect object
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
  o<<(float)i<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:ff<int>*2",
                 // SELECT statement
              secondary_db // connect object
             ); 
   // create select stream
 
 float f1;
 char f2[31];

 i<<8<<8; // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

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

  main_db.rlogon("UID=scott;PWD=tiger;DSN=freetds_mssql"); // connect to ODBC

  otl_cursor::direct_exec
   (
    main_db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    main_db,
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table

   // opening a secondary connect
   secondary_db.rlogon
     (main_db.get_connect_struct().get_henv(), // environment handle to the main database connect
      main_db.get_connect_struct().get_hdbc() // database connect handle to the main database connect
    ); // connect to ODBC

   select(); // select records from table

  // releasing the secondary connect
  secondary_db.logoff(); 

 }catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 main_db.logoff(); // disconnect from ODBC

 return 0;

}
