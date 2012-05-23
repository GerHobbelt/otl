#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#define OTL_ORA7 // Compile OTL 4.0/OCI9i
#define OTL_STREAM_NO_PRIVATE_UNSIGNED_LONG_OPERATORS
#define OTL_NUMERIC_TYPE_1_ID "ULONG"
#define OTL_NUMERIC_TYPE_1_STR_SIZE 40
#define OTL_NUMERIC_TYPE_1 unsigned long 
#define OTL_STR_TO_NUMERIC_TYPE_1(str,n)        \
{                                               \
  sscanf(str,"%lu",&n);                         \
}

#define OTL_NUMERIC_TYPE_1_TO_STR(n,str)        \
{                                               \
  sprintf(str,"%lu",n);                         \
}

#define OTL_NUMERIC_TYPE_2_ID "UULONG"
#define OTL_NUMERIC_TYPE_2_STR_SIZE 40
#define OTL_NUMERIC_TYPE_2 unsigned long long
#define OTL_STR_TO_NUMERIC_TYPE_2(str,n)        \
{                                               \
  sscanf(str,"%llu",&n);                        \
}

#define OTL_NUMERIC_TYPE_2_TO_STR(n,str)        \
{                                               \
  sprintf(str,"%llu",n);                         \
}

#define OTL_BIGINT long long
#define OTL_STR_TO_BIGINT(str,n)                \
{                                               \
  sscanf(str,"%lld",&n);                         \
}

#define OTL_BIGINT_TO_STR(n,str)                \
{                                               \
  sprintf(str,"%lld",n);                        \
}

#define OTL_STREAM_READ_ITERATOR_ON
const OTL_NUMERIC_TYPE_1 VAL1=1234567890;
const OTL_NUMERIC_TYPE_2 VAL2=1234567890;


#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<uulong>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];
 
 for(OTL_NUMERIC_TYPE_2 i=VAL2;i<=VAL2+100;++i){
  int ndx=static_cast<int>(i-VAL2);
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",ndx);
#else
  sprintf(tmp,"Name%d",ndx);
#endif
#else
  sprintf(tmp,"Name%d",ndx);
#endif
  o<<i;
  o<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
//               "begin "
//               "open :cur for "
              "select f1 :#1<ulong>, f2 "
                // the default mapping of f1 needs to be overriden 
                // explicitly when bigint's are used in a combination
                // OTL/OCIx, because the default mapping maps
                // Oracle NUMBERs into double containers, which are not 
                // big enough to hold 64-bit signed integer values.
              "from test_tab "
              "where f1>=:f<ulong> "
              "  and f1<=:ff<ulong> ",
//               "end;",
              db
//               ":cur"
             ); 
   // create select stream
 
 OTL_NUMERIC_TYPE_1 f1;
 char f2[31];
#if defined(_MSC_VER)
// char f1str[40];
#endif

 i<<VAL1+8
  <<VAL1+16; // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1=";
#if 0
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  _i64toa_s(f1,f1str,sizeof(f1str),10);
#else
  _i64toa(f1,f1str,10);
#endif
  cout<<f1str<<", f2="<<f2<<endl;
#elif defined(__GNUC__)
  cout<<f1<<", f2="<<f2<<endl;
#endif
#endif
  cout<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

    db.rlogon("scott/tiger"); // connect to Oracle
//   db.rlogon("sa/qpasa@sqlloc"); 

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 varchar(30))"
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
