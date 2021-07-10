#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>
#if !defined(ORA_VER_DEFINED)
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
#endif
#define OTL_UNICODE // Enable Unicode OTL for OCI8i
#define OTL_STREAM_WITH_STD_UNICODE_CHAR_ARRAY_ON
#if !defined(OTL_CPP_14_ON)
#define OTL_CPP_14_ON
#endif
#include <otlv4.h> // include the OTL 4.0 header file

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma warning(disable : 28199)
#endif

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];
 std::array<char16_t,31> tmp2; // Null terminated Unicode character array.

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  char16_t* c2=&tmp2[0];
  char* c1=tmp;
// Unicode's first 128 characters are ASCII (0..127), so
// all is needed for converting ASCII into Unicode is as follows:
  while(*c1){
    *c2=OTL_SCAST(unsigned char,*c1);
   ++c1; ++c2;   
  }  
  *c2=0; // target Unicode string is null terminated,
         // only the null terminator is a two-byte character, 
         // not one-byte
  o<<i;
  o<<tmp2; 
 }

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1=0;
 std::array<char16_t,31> f2;

 i<<8; // assigning :f = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  i>>f2;
  cout<<"f1="<<f1<<", f2=";
// Unicode's first 128 characters are ASCII, so in order
// to convert Unicode back to ASCII all is needed is
// as follows:
   for(size_t j=0;f2[j]!=0;++j){
     cout<<OTL_SCAST(char,f2[j]);
   }
   cout<<endl;
 }

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
   i>>f1>>f2;
  cout<<"f1="<<f1<<", f2=";
  for(size_t j=0;f2[j]!=0;++j){
     cout<<OTL_SCAST(char,f2[j]);
   }
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
    "create table test_tab(f1 number, f2 varchar2(60))"
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
