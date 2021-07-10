#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA9I // Compile OTL 4.0/OCI9i

// Uncomment the line below in case of OCI8.1.7 that 
// used against Oracle 9i.
//#define OTL_ORA8I

#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON)
// enable OTL support for std::optional when clang or g++ is used and
// when -std=c++14 or higher switch is used
#define OTL_STREAM_WITH_STD_OPTIONAL_ON
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#define OTL_STREAM_WITH_STD_OPTIONAL_ON
#endif

#define OTL_ORA_TIMESTAMP
#include <otlv4.h> // include the OTL 4.0 header file

#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
#include <experimental/optional>
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#include <optional>
#endif

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(3, // stream buffer size in logical rows.
              "insert into test_tab "
              "values(:f1<int>,:f2<timestamp>)", 
                 // INSERT statement
              db // connect object
             );

 otl_datetime tm;

 for(int i=1;i<=10;++i){
  tm.year=1998;
  tm.month=10;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
  tm.frac_precision=6; // microseconds
  tm.fraction=123456;
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
  std::experimental::optional<otl_datetime> tm2;
  if(i!=5)
    tm2=tm;
  o<<i<<tm2;
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
  std::optional<otl_datetime> tm2;
  if(i!=5)
    tm2=tm;
  o<<i<<tm2;
#else
  if(i!=5)
    o<<i<<tm;
  else
    o<<i<<otl_null();
#endif
 }
}

void select(void)
{ 
 otl_stream i(5, // stream buffer size in logical rows
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
 std::experimental::optional<int> f1;
 std::experimental::optional<otl_datetime> f2;
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
 std::optional<int> f1;
 std::optional<otl_datetime> f2;
#else
 int f1=0;
 otl_datetime f2;
#endif
 
 while(!i.eof()){ // while not end-of-data

#if (defined(__clang__) || defined(__GNUC__)) && defined(OTL_CPP_14_ON) \
  && defined(OTL_STREAM_WITH_STD_OPTIONAL_ON)
// for clang or g++, when they use -std=c++14 or higher
 // Second's precision needs to be specified BEFORE the stucture can be
 // used either for writnig or reading timestamp values, which have second's 
 // fractional part.
   f2.emplace(otl_datetime());
   f2->frac_precision=6; // microseconds
   i>>f1>>f2;
   cout<<"f1="<<*f1;
   if(!f2)
     cout<<", f2=NULL";
   else
     cout<<", f2="<<f2->month<<"/"<<f2->day<<"/"
         <<f2->year<<" "<<f2->hour<<":"<<f2->minute<<":"
         <<f2->second<<"."
         <<f2->fraction;
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
   f2.emplace(otl_datetime());
   f2->frac_precision=6; // microseconds
   i>>f1>>f2;
   cout<<"f1="<<*f1;
   if(!f2)
     cout<<", f2=NULL";
   else
     cout<<", f2="<<f2->month<<"/"<<f2->day<<"/"
         <<f2->year<<" "<<f2->hour<<":"<<f2->minute<<":"
         <<f2->second<<"."
         <<f2->fraction;
#else
 // Second's precision needs to be specified BEFORE the stucture can be
 // used either for writnig or reading timestamp values, which have second's 
 // fractional part.
   f2.frac_precision=6; // microseconds
   i>>f1>>f2;
   cout<<"f1="<<f1;
   if(i.is_null())
     cout<<", f2=NULL";
   else
     cout<<", f2="<<f2.month<<"/"<<f2.day<<"/"
         <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
         <<f2.second<<"."
         <<f2.fraction;
#endif
  cout<<endl;
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
    "create table test_tab(f1 int, f2 timestamp)"
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

 db.logoff(); // disconnect from Oracle

 return 0;

}
