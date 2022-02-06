#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G
//#define OTL_ORA10G_R2

#define OTL_UNICODE // Enable Unicode OTL for OCI9i

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
 db.set_max_long_size(4);
 otl_stream o(1, // buffer size should be set to 1
              "insert into test_tab values(:f1<int>,empty_clob()) "
              "returning f2 into :f2<nclob>",
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
 db.set_max_long_size(4);
 otl_stream i(50, // buffer size can be > 1
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

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("system/oracle@myora_tns"); // connect to the database

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
    "create table test_tab(f1 int, f2 nclob)"
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
