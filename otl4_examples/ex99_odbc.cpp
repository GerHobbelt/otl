#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <string>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#define OTL_STL // Turn on STL features and otl_value<T>
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif

#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON)
// enable OTL support for std::optional when clang or g++ is used and
// when -std=c++14 or higher switch is used
#define OTL_STREAM_WITH_STD_OPTIONAL_ON
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#define OTL_STREAM_WITH_STD_OPTIONAL_ON
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1929) && (defined(OTL_CPP_20_ON))
// VC++ 2022 or higher when /std=c++20 is used
#define OTL_STREAM_WITH_STD_OPTIONAL_ON
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1946) && defined(OTL_CPP_23_ON)
// VC++ 2026 or higher when /std=c++23preview or higher is used
#define OTL_STREAM_WITH_STD_OPTIONAL_ON
#endif

#include <otlv4.h> // include the OTL 4 header file

#if (defined(__clang__) && (__clang_major__*100+__clang_minor__ < 900) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
#include <experimental/optional>
#endif

#if (defined(__clang__) && (__clang_major__*100+__clang_minor__ >= 900)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
#include <optional>
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#include <optional>
#endif

using namespace std;

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab "
              "values(:f1<int>,:f2<char[31]>,:f3<timestamp>)", 
                 // SQL statement
              db // connect object
             );
 otl_compact_value<int,-1> f1; // otl_compact_value container for int, and -1 as NULL value
 otl_value<string> f2; // otl_value container of STL string
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
 std::experimental::optional<otl_datetime> f3; // container of otl_datetime
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
 std::optional<otl_datetime> f3; // container of otl_datetime
#else
 otl_value<otl_datetime> f3; // container of otl_datetime
#endif


 for(int i=1;i<=100;++i){
   f1=i;
   if(i%2==0)
     f2="NameXXX";
   else
     f2=otl_null(); // Assign otl_null() to f2
   
   if(i%3==0){
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
     f3.emplace(otl_datetime(2001,1,1,13,10,5));
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
     f3.emplace(otl_datetime(2001,1,1,13,10,5));
#else
    // Assign a value to f3 via the .v field directly
     f3.v.year=2001;
     f3.v.month=1;
     f3.v.day=1;
     f3.v.hour=13;
     f3.v.minute=10;
     f3.v.second=5;
     f3.set_non_null(); // Set f3 as a "non-NULL"
#endif
   }else
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
     f3=std::experimental::optional<otl_datetime>();
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
     f3=std::optional<otl_datetime>();
#else
     f3.set_null(); // Set f3 as null via .set_null() function
#endif
   
#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
   otl_write_row(o,f1,f2,f3);
#else
   // when variadic template functions are not supported by the C++
   // compiler, OTL provides nonvariadic versions of the same template
   // functions in the range of [1..15] parameters
   otl_write_row(o,f1,f2,f3);
   // the old way (operators >>() / <<()) is available as always:
   //  o<<f1<<f2<<f3;
#endif
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream

 otl_compact_value<int,-1> f1(0);
 otl_value<string> f2;
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
 std::experimental::optional<otl_datetime> f3;
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
 std::optional<otl_datetime> f3;
#else
 otl_value<otl_datetime> f3;
#endif


#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
 otl_write_row(i,8,8); // assigning :f11, :f12 = 8
#else
  // when variadic template functions are not supported by the C++
  // compiler, OTL provides nonvariadic versions of the same template
  // functions in the range of [1..15] parameters
 otl_write_row(i,8,8); // assigning :f11, :f12 = 8
  // the old way (operators >>() / <<()) is available as always:
  // i<<8<<8; // assigning :f11, :f12 = 8
#endif
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
   otl_read_row(it,f1,f2,f3);
#else
  // when variadic template functions are not supported by the C++
  // compiler, OTL provides nonvariadic versions of the same template
  // functions in the range of [1..15] parameters
   otl_read_row(it,f1,f2,f3);
  // the old way (operators >>() / <<()) is available as always:
  //  it>>f1>>f2>>f3;
#endif
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
   if(f3)
     cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<*f3<<endl;
   else
     cout<<"f1="<<f1<<", f2="<<f2<<", f3=NULL"<<endl;
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
   if(f3)
     cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<*f3<<endl;
   else
     cout<<"f1="<<f1<<", f2="<<f2<<", f3=NULL"<<endl;
#else
  cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<f3<<endl;
#endif
 
}

 i<<4<<4; // assigning :f11, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2>>f3;
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
   if(f3)
     cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<*f3<<endl;
   else
     cout<<"f1="<<f1<<", f2="<<f2<<", f3=NULL"<<endl;
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
   if(f3)
     cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<*f3<<endl;
   else
     cout<<"f1="<<f1<<", f2="<<f2<<", f3=NULL"<<endl;
#else
  cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<f3<<endl;
#endif
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to MS SQL Server

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30), f3 datetime)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from MS SQL Server

 return 0;

}
