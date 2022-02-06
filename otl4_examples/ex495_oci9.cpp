#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
#define OTL_ORA_UTF8 // Enable UTF8 OTL for OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

// Sample UTF8 based string
unsigned char utf8_sample[]={0x61,0x62,0x63,0xd0,0x9e,0xd0,0x9b,0xd0,0xac,0x0};

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab "
               "values(:f1<int>,:f2<char[31]>,:f3<nchar[31]>)", 
                 // SQL statement
              db // connect object
             );
 unsigned char tmp[31];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
   strcpy_s(reinterpret_cast<char*>(tmp),
            sizeof(tmp),
            reinterpret_cast<const char*>(utf8_sample));
#else
   strcpy(reinterpret_cast<char*>(tmp),reinterpret_cast<const char*>(utf8_sample));
#endif
#else
   strcpy(reinterpret_cast<char*>(tmp),reinterpret_cast<const char*>(utf8_sample));
#endif
   o<<i;
   o<<tmp;
   o<<tmp;
 }

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select "
               "  f1, f2, f3 "
              "from test_tab "
              "where f1>=:f<int> "
              "  and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 unsigned char f2[31]={0};
 unsigned char f3[31]={0};

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 int j;
 while(!i.eof()){ // while not end-of-data
   i>>f1>>f2>>f3;
   cout<<"f1="<<f1<<", f2=";
   for(j=0;f2[j]!=0;++j)
     printf("%2x ", f2[j]);
   cout<<", f3=";
   for(j=0;f3[j]!=0;++j)
     printf("%2x ", f3[j]);
   cout<<endl;
 }

}

int main()
{
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
#if defined(_MSC_VER) && (_MSC_VER==1700)
#pragma warning (disable:6031)
#endif
  (void)_putenv(const_cast<char*>("NLS_LANG=.AL32UTF8")); 
  // set your Oracle Client NLS_LANG 
  // if its default was set to something else
#else
  putenv(const_cast<char*>("NLS_LANG=.AL32UTF8")); // set your Oracle Client NLS_LANG 
                                // if its default was set to something else
#endif
#else
  putenv(const_cast<char*>("NLS_LANG=.AL32UTF8")); // set your Oracle Client NLS_LANG 
                                // if its default was set to something else
#endif

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
    "create table test_tab(f1 number, f2 varchar2(60), f3 nvarchar2(60))"
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
