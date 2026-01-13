#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
// Suppress VC++ warnings about insecure sprintf / sscanf calls.
#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER) && (_MSC_VER==1700)
#pragma warning (disable:6031)
#endif

#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA8 // Compile OTL 4.0/OCI8

// disable private operators >>/<< for unsigned long
#define OTL_STREAM_NO_PRIVATE_UNSIGNED_LONG_OPERATORS 

// #defines for extended numeric data types
#define OTL_NUMERIC_TYPE_1_ID "ULONG"
#define OTL_NUMERIC_TYPE_1_STR_SIZE 40
#define OTL_NUMERIC_TYPE_1 unsigned long 
#define OTL_STR_TO_NUMERIC_TYPE_1(str,n)        \
{                                               \
  (void)sscanf(str,"%lu",&n);                   \
}

#define OTL_NUMERIC_TYPE_1_TO_STR(n,str)        \
{                                               \
  sprintf(str,"%lu",n);                         \
}

#define OTL_NUMERIC_TYPE_2_ID "UULONG"
#define OTL_NUMERIC_TYPE_2_STR_SIZE 40
#if defined(_MSC_VER) && (_MSC_VER==1200)
#define OTL_NUMERIC_TYPE_2 __int64

#define OTL_STR_TO_NUMERIC_TYPE_2(str,n)        \
{                                               \
  n=_atoi64(str);                               \
}

#define OTL_NUMERIC_TYPE_2_TO_STR(n,str)        \
{                                               \
  _i64toa(n,str,10);                            \
}

#else
#define OTL_NUMERIC_TYPE_2 unsigned long long

#define OTL_STR_TO_NUMERIC_TYPE_2(str,n)        \
{                                               \
  (void)sscanf(str,"%llu",&n);                  \
}

#define OTL_NUMERIC_TYPE_2_TO_STR(n,str)        \
{                                               \
  sprintf(str,"%llu",n);                        \
}
#endif

#define OTL_NUMERIC_TYPE_3_ID "LDOUBLE"
#define OTL_NUMERIC_TYPE_3_STR_SIZE 60
#define OTL_NUMERIC_TYPE_3 long double
#define OTL_STR_TO_NUMERIC_TYPE_3(str,n)        \
{                                               \
  (void)sscanf(str,"%Lf",&n);                   \
}

#define OTL_NUMERIC_TYPE_3_TO_STR(n,str)        \
{                                               \
  sprintf(str,"%Lf",n);                         \
}

#include <otlv4.h> // include the OTL 4.0 header file

const OTL_NUMERIC_TYPE_1 VAL1=1234567890;
const OTL_NUMERIC_TYPE_2 VAL2=1234567890;
const OTL_NUMERIC_TYPE_3 VAL3=1234567890;

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
  sprintf(tmp,"Name%d",ndx);
  o<<i;
  o<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<ulong>, f2 "
              "from test_tab "
              "where f1>=:f<ulong> "
              "  and f1<=:ff<ulong>",
              db
             ); 
   // create select stream
 
 OTL_NUMERIC_TYPE_1 f1=0;
 char f2[31];

 i<<VAL1+8
  <<VAL1+16;

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 
}

}

void select2()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<ldouble>, f2 "
              "from test_tab "
              "where f1>=:f<ldouble> "
              "  and f1<=:ff<ldouble>",
              db
             ); 
   // create select stream
 
 OTL_NUMERIC_TYPE_3 f1=0;
 char f2[31];

 i<<VAL3+8
  <<VAL3+16;

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
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
    "create table test_tab(f1 number, f2 varchar2(30))"
    );  // create table

  insert(); 
  select(); 
  select2(); 
 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
