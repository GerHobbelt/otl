#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
#define OTL_UNICODE // Enable Unicode OTL for ODBC

#if defined(__GNUC__)

namespace std{
   typedef unsigned short unicode_char;
   typedef basic_string<unicode_char> unicode_string;
}

#define OTL_UNICODE_CHAR_TYPE unicode_char
#define OTL_UNICODE_STRING_TYPE unicode_string

#else

#define OTL_UNICODE_CHAR_TYPE wchar_t
#define OTL_UNICODE_STRING_TYPE wstring
#endif

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 db.set_max_long_size(80000); // set maximum long string size for connect object
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<varchar_long>) ",
                 // SQL statement
              db // connect object
             );
OTL_UNICODE_CHAR_TYPE tmp[80001]; // Null terminated Unicode character array.

 for(int i=1;i<=20;++i){
  tmp[0]='<';
  tmp[1]='T';
  tmp[2]='A';
  tmp[3]='G';
  tmp[4]='>';
  tmp[5]=1111; // Unicode character (decimal code of 1111)  
  tmp[6]=2222; // Unicode character (decimal code of 2222)
  tmp[7]=3333; // Unicode character (decimal code of 3333)  
  tmp[8]=4444; // Unicode character (decimal code of 4444)  
  tmp[9]='<';
  tmp[10]='/';
  tmp[11]='T';
  tmp[12]='A';
  tmp[13]='G';
  tmp[14]='>';
  tmp[15]=0;
  OTL_UNICODE_STRING_TYPE tmp_str(tmp);
  o<<i<<tmp_str;
 }

}

void select()
{ 
 OTL_UNICODE_STRING_TYPE f2;
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
 
 int f1=0;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8 
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2=";
  for(size_t j=0;j<f2.length();++j)
    if(f2[j]<128)
      cout<<static_cast<char>(f2[j]);
    else
      cout<<static_cast<int>(f2[j])<<" ";
  cout<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to MS SQL

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

 db.logoff(); // disconnect from MS SQL

 return 0;

}
