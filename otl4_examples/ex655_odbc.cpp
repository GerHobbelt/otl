#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

// Compile OTL 4.0/ODBC in Windows. Informix CLI uses the  
// regular ODBC header files in Windows
#define OTL_ODBC 

// Uncomment the #defines below in Linux / Unix
//#define OTL_ODBC_UNIX
//#define OTL_INFORMIX_CLI

#if defined(_MSC_VER)||defined(__BORLANDC__) // VC++

// Enabling support for 64-bit signed integers
// Since 64-bit integers are not part of the ANSI C++
// standard, this definition is compiler specific.
#define OTL_BIGINT __int64

// Defining a bigint constant that is larger than
// the max 32-bit integer value.
const OTL_BIGINT BIGINT_VAL1=12345678901234000;

#elif defined(__GNUC__) // GNU C++

#include <stdlib.h>

// Enabling support for 64-bit signed integers
// Since 64-bit integers are not part of the ANSI C++
// standard, this definition is compiler specific.
#define OTL_BIGINT long long

const OTL_BIGINT BIGINT_VAL1=12345678901234000LL;

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

 for(OTL_BIGINT i=BIGINT_VAL1;i<=BIGINT_VAL1+100;++i){
  int ndx=static_cast<int>(i-BIGINT_VAL1);
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",ndx);
#else
  sprintf(tmp,"Name%d",ndx);
#endif
#else
  sprintf(tmp,"Name%d",ndx);
#endif
  o<<i<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<bigint>, f2 "
                // the default mapping of f1 needs to be overriden 
                // explicitly
              "from test_tab "
              "where f1>=:f<bigint> "
              "  and f1<=:ff<bigint>",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 OTL_BIGINT f1=0;
 char f2[31];

 i<<BIGINT_VAL1+8
  <<BIGINT_VAL1+16; // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1=";
#if defined(__BORLANDC__) || defined(_MSC_VER)
  char f1str[40];
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  _i64toa_s(f1,f1str,sizeof(f1str),10);
#else
  _i64toa(f1,f1str,10);
#endif
#else
  _i64toa(f1,f1str,10);
#endif
  cout<<f1str<<", f2="<<f2<<endl;
#else
  cout<<f1<<", f2="<<f2<<endl;
#endif
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize Informix CLI environment
 try{

  db.rlogon("informix/tigger@informixsql"); // connect to Informix

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int8, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Informix

 return 0;

}
