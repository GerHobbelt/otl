#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
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
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[5]>)", 
                 // SQL statement, char[5] means 5 2-byte 
                 // Unicode charatcters including a null
                 // terminator
              db // connect object
             );

 OTL_UNICODE_CHAR_TYPE tmp[5]; // Null terminated Unicode character array.

 for(int i=1;i<=100;++i){
  o<<i;
  tmp[0]=1111; // Unicode character (decimal code of 1111)
  tmp[1]=2222; // Unicode character (decimal code of 2222)
  tmp[2]=3333; // Unicode chracater (decimal code of 3333)
  tmp[3]=4444; // Unicode chracater (decimal code of 4444)
  tmp[4]=0; // Unicode null terminator 
  o<<tmp; 
 }

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab "
              "where f1>=:f11<int> "
              "  and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 OTL_UNICODE_CHAR_TYPE f2[5];

 i<<8<<8; // assigning :f11 = 8, f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  i>>f2;
  cout<<"f1="<<f1<<", f2=";
   for(int j=0;f2[j]!=0;++j)
     cout<<" "<<f2[j];
   cout<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2=";
   for(int j=0;f2[j]!=0;++j)
     cout<<" "<<f2[j];
   cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@sapdbsql2"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table
   db.commit();
  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(4) unicode)"
    );  // create table
  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
