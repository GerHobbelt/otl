#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4/DB2 CLI
#define OTL_UNICODE // Enable Unicode OTL for ODBC
#if defined(__GNUC__)
#define OTL_UNICODE_CHAR_TYPE unsigned short 
#else
#define OTL_UNICODE_CHAR_TYPE wchar_t
#endif
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
  otl_long_unicode_string f2(80000); // define long unicode string variable
 db.set_max_long_size(80000); // set maximum long string size for connect object
 otl_stream o(10, // buffer size 
              "insert into test_tab values(:f1<int>,:f2<varchar_long>) ",
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=20;++i){
  f2[0]='<';
  f2[1]='T';
  f2[2]='A';
  f2[3]='G';
  f2[4]='>';
  f2[5]=1111; // Unicode character (decimal code of 1111)  
  f2[6]=2222; // Unicode character (decimal code of 2222)
  f2[7]=3333; // Unicode character (decimal code of 3333)  
  f2[8]=4444; // Unicode character (decimal code of 4444)  
  f2[9]='<';
  f2[10]='/';
  f2[11]='T';
  f2[12]='A';
  f2[13]='G';
  f2[14]='>';
  f2.set_len(15);
  o<<i<<f2;
 }

}

void select()
{ 
 otl_long_unicode_string f2(80000); // define long string variable
 db.set_max_long_size(80000); // set maximum long string size for connect object

 otl_stream i(10, // buffer size
              "select f1, f2 "
              "from test_tab "
              "where f1>=:f11<int> "
              " and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8 
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2=";
  for(int j=0;j<f2.len();++j)
    if(f2[j]<128)
      cout<<static_cast<char>(f2[j]);
    else
      cout<<static_cast<int>(f2[j])<<" ";
  cout<<endl; 
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2 CLI environment
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
    "create table test_tab(f1 int, f2 xml)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from DB2

 return 0;

}
