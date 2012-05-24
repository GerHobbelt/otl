#include <iostream>
#include <stdio.h>
using namespace std;

#define OTL_ODBC
//#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC

#if defined(_MSC_VER) || defined(__BORLANDC__) // VC++ || Borland C++

// Enabling support for 64-bit signed integers
// Since 64-bit integers are not part of the ANSI C++
// standard, this definition is compiler specific.
#define OTL_BIGINT __int64

// Defining a string-to-bigint conversion 
// that is used by OTL internally.
// Since 64-bit ineteger conversion functions are
// not part of the ANSI C++ standard, the code
// below is compiler specific
#define OTL_STR_TO_BIGINT(str,n)                \
{                                               \
  n=_atoi64(str);                               \
}

// Defining a bigint-to-string conversion 
// that is used by OTL internally.
// Since 64-bit ineteger conversion functions are
// not part of the ANSI C++ standard, the code
// below is compiler specific
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  _i64toa_s(n,str,sizeof(str),10);              \
}
#else
#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  _i64toa(n,str,10);                            \
}
#endif

#elif defined(__GNUC__) // GNU C++

#include <stdlib.h>

// Enabling support for 64-bit signed integers
// Since 64-bit integers are not part of the ANSI C++
// standard, this definition is compiler specific.
#define OTL_BIGINT long long

// Defining a string-to-bigint conversion 
// that is used by OTL internally.
// Since 64-bit ineteger conversion functions are
// not part of the ANSI C++ standard, the code
// below is compiler specific.
#define OTL_STR_TO_BIGINT(str,n)                \
{                                               \
  n=strtoll(str,0,10);                          \
}

// Defining a bigint-to-string conversion 
// that is used by OTL internally.
// Since 64-bit ineteger conversion functions are
// not part of the ANSI C++ standard, the code
// below is compiler specific
#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  sprintf(str,"%lld",n);                        \
}


#endif

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<bigint>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(OTL_BIGINT i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
   sprintf_s(tmp,sizeof(tmp),"Name%d",static_cast<int>(i));
#else
   sprintf(tmp,"Name%d",static_cast<int>(i));
#endif
#else
   sprintf(tmp,"Name%d",static_cast<int>(i));
#endif
  o<<i<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<bigint> and f1<=:ff<bigint>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 OTL_BIGINT f1;
 char f2[31];

 i<<static_cast<OTL_BIGINT>(8)<<static_cast<OTL_BIGINT>(8);
 // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1=";
  cout<<(int)f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tigger@sybsql"); // connect to ODBC

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 bigint, f2 varchar(30))"
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
