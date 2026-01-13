#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <string>
using namespace std;

#define OTL_ORA8I // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8i
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_STL // Enable STL compatibily mode

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

otl_connect db; // connect object

void insert()
// insert rows into table
{

 string f2; 
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "insert into test_tab values(:f1<int>,:f2<varchar_long>)",
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=20;++i){
  f2.assign(50001,' '); 
  for(size_t j=0;j<50000;++j)
   f2[j]='*';
  f2[50000]='?';
#if defined(OTL_STD_STRING_VIEW_CLASS)
  OTL_STD_STRING_VIEW_CLASS f2_sv(f2.c_str(),f2.length());
  o<<i<<f2_sv;
#else
  o<<i<<f2;
#endif
 }

}

void select()
{ 
 string f2;
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream i(1, // buffer size needs to be set to 1 for operations with LOBs
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1=0;

 i<<8; // assigning :f = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.length()-1]<<", len="
      <<static_cast<int>(f2.length())<<endl;
 
}

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.length()-1]<<", len="
      <<static_cast<int>(f2.length())<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("system/oracle@myora_tns"); // connect to Oracle

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 long)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
