#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/SNAC 10 or higher
#define OTL_UNICODE // Enable Unicode OTL for DB2-CLI

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

#define OTL_STREAM_POOLING_ON 
 // turn on OTL stream pooling.
 // #define OTL_STREAM_POOLING_ON line 
 // can be commented out, the number of iteration in
 // the select() loop can be increased, and the difference 
 // in performace with and without OTL_STREAM_POOLING_ON can
 // be benchmarked. The difference should increase with the overall
 // number of streams to be used in one program.

#if (defined(__clang__) && (__clang_major__*100+__clang_minor__ >= 900)) && \
     (defined(OTL_CPP_14_ON) || defined(OTL_CPP_17_ON))
#include <string_view>
#define OTL_UNICODE_STD_STRING_VIEW_CLASS std::basic_string_view<unicode_char>
#elif (defined(__clang__) && (__clang_major__*100+__clang_minor__ < 900) || defined(__GNUC__)) && \
     (defined(OTL_CPP_14_ON) || defined(OTL_CPP_17_ON))
#include <experimental/string_view>
#define OTL_UNICODE_STD_STRING_VIEW_CLASS std::experimental::basic_string_view<unicode_char>
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#include <string_view>
#define OTL_UNICODE_STD_STRING_VIEW_CLASS std::basic_string_view<wchar_t>
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

 OTL_UNICODE_CHAR_TYPE tmp[5];
 OTL_UNICODE_STRING_TYPE tmp2;

 for(int i=1;i<=100;++i){
  o<<i;
  tmp[0]=1111; // Unicode character (decimal code of 1111)
  tmp[1]=2222; // Unicode character (decimal code of 2222)
  tmp[2]=3333; // Unicode chracater (decimal code of 3333)
  tmp[3]=4444; // Unicode chracater (decimal code of 4444)
  tmp[4]=0; // Unicode null terminator 
  tmp2=tmp;
#if defined(OTL_STD_UNICODE_STRING_VIEW_CLASS)
  OTL_STD_UNICODE_STRING_VIEW_CLASS tmp2_sv(tmp2.c_str(),tmp2.length());
  o<<tmp2_sv; 
#else
  o<<tmp2; 
#endif
 }

}

void select()
{ 
  // when this function is called in a loop, on the second iteration
  // of the loop the stream i will will be get the instances of the
  // OTL stream from the stream pool, "fast reopen", so to speak.

 otl_stream i(50, // buffer size
              "select * from test_tab "
              "where f1>=:f11<int> "
              "  and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 OTL_UNICODE_STRING_TYPE f2;

 i<<8<<8; // assigning :f11 = 8, f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  i>>f2;
  cout<<"f1="<<f1<<", f2=";
   for(size_t j=0;j<f2.length();++j)
     cout<<" "<<f2[j];
   cout<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2=";
   for(size_t j=0;j<f2.length();++j)
     cout<<" "<<f2[j];
   cout<<endl;
 }

}
// destructor of stream i will call the close() function and the OTL
// stream instance will be placed in the stream pool.

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to the database
#ifdef OTL_STREAM_POOLING_ON
  db.set_stream_pool_size(2); 
   // set the maximum stream pool size and actually initializes 
   // the stream pool.
   // if this function is not called, the stream pool
   // gets initialized anyway, with the default size of 32 entries.
#endif

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
    "create table test_tab(f1 int, f2 nvarchar(11))"
    );  // create table
  insert(); // insert records into table
  for(int i=1;i<=10;++i){
    cout<<"======================> Iteration "<<i<<endl;
    select(); // select records from table
  }

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
