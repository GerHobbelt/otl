#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4/ OCI
#define OTL_STREAM_WITH_STD_TUPLE_ON
#define OTL_STREAM_WITH_STD_VARIANT_ON
#define OTL_STREAM_WITH_STD_CHAR_ARRAY_ON
#if !defined(OTL_CPP_17_ON)
#define OTL_CPP_17_ON
#endif

#if defined(OTL_CPP_20_ON)
// enable OTL support for std::span
#define OTL_STREAM_WITH_STD_SPAN_ON
#endif

#include <otlv4.h> // include the OTL 4 header file


otl_connect db; // connect object

using Variant=std::variant<int,float>;
using Tuple=std::tuple<Variant,std::array<char,32>>;

#if defined(OTL_CPP_20_ON)
using Span=std::span<Tuple>;
#endif

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 Tuple t;

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER) && (_MSC_VER >= 1400) // VC++ 8.0 or higher
   sprintf_s(std::get<1>(t).data(),std::get<1>(t).size(),"Name%d",i);
#else
   sprintf(std::get<1>(t).data(),"Name%d",i);
#endif
  std::get<0>(t)=i;
#if defined(OTL_CPP_20_ON)
  o<<Span(&t,1);
#else
  o<<t;
#endif

 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<int>, f2 "
              "from test_tab where f1>=:f1_1<int> and f1<=:f1_2<int>*2",
                 // SELECT statement
                 // for Oracle, OTL maps NUMBER to double, so
                 // the default mapping needs to be overridden on SELECT 
                 // (see :#1<int>) for the data types defined in the Variant
                 // to match the data types on the SELECT statement
              db // connect object
             ); 
   // create select stream
 
 Tuple t;
 int f1;
 char f2[32];

 i<<8<<8;

 for(auto& it : i){
   it>>t;
   const Variant& v=std::get<0>(t);
   cout<<"f1="<<std::get<0>(v)<<", f2="<<std::get<1>(t).data()<<endl;
 }

 i<<4<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
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
    "create table test_tab(f1 int, f2 varchar(31))"
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
