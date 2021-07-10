#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_STL
#define OTL_DB2_CLI // Compile OTL 4.0/DB2 CLI 

#if (defined(__clang__) && (__clang_major__*100+__clang_minor__ >= 900)) && \
     (defined(OTL_CPP_14_ON))
#include <experimental/string_view>
#define OTL_STD_STRING_VIEW_CLASS std::experimental::string_view
#elif (defined(__clang__) && (__clang_major__*100+__clang_minor__ >= 900)) && \
     (defined(OTL_CPP_17_ON))
#include <string_view>
#define OTL_STD_STRING_VIEW_CLASS std::string_view
#elif (defined(__clang__) && (__clang_major__*100+__clang_minor__ < 900) || defined(__GNUC__)) && \
     (defined(OTL_CPP_14_ON) || defined(OTL_CPP_17_ON))
#include <experimental/string_view>
#define OTL_STD_STRING_VIEW_CLASS std::experimental::string_view
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#include <string_view>
#define OTL_STD_STRING_VIEW_CLASS std::string_view
#endif

#include <otlv4.h> // include the OTL 4.0 header file

string str_val("123456" "\0" "7890", 11); // std::string with a null terminator in the middle.
otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<raw[30]>)", 
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=4;++i){
#if defined(OTL_STD_STRING_VIEW_CLASS)
  OTL_STD_STRING_VIEW_CLASS str_val_sv(str_val.c_str(),str_val.length());
  o<<i<<str_val_sv;
#else
  o<<i<<str_val;
#endif
 }
}

void select()
{ 
 otl_stream i(10, // buffer size
              "select f1, f2 :#2<raw[30]> "
              "from test_tab "
              "where f2>=:f2in<raw[30]>",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 string f2;

 i<<str_val; // assigning :f2in = str_val
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2=";
  for(size_t j=0;j<f2.length();++j)
    if(f2[j]==0)
      cout<<"\\0";
    else
      cout<<f2[j];
  cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
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
    "create table test_tab(f1 int, f2 varchar(30))"
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
