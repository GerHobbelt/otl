#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_ORA_UTF8 // Enable UTF8 OTL for OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

// Sample UTF8 based string
unsigned char utf8_sample[]=
{0x61,0x62,0x63,0xd0,0x9e,0xd0,0x9b,0xd0,
 0xac,0xd0,0x93,0xd0,0x90,0x0};

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
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
 }

}

void select()
{ 
 otl_stream i(1, // buffer size
              "begin "
              " open :cur<refcur,out[50]> for "
     // :cur is a bind variable name, refcur -- its type, 
     // out -- output parameter, 50 -- the buffer size when this
     // reference cursor will be attached to otl_refcur_stream
              "  select * "
              "  from test_tab "
              "  where f1>=:f<int,in> and f1<=:f*2; "
              "end;", // PL/SQL block returns a referenced cursor
              db // connect object
             ); 
   // create select stream with referenced cursor

  i.set_commit(0); // set stream "auto-commit" to OFF.
 
 int f1=0;
 unsigned char f2[31]={0};

 otl_refcur_stream s; // reference cursor stream for reading rows.

 i<<8; // assigning :f = 8
 i>>s; // initializing the refrence cursor stream with the output 
       // reference cursor.

 while(!s.eof()){ // while not end-of-data
   s>>f1;
   s>>f2;
   cout<<"f1="<<f1<<", f2=";
   for(int j=0;f2[j]!=0;++j)
     printf("%2x ", f2[j]);
   cout<<endl;
 
}

 s.close(); // closing the reference cursor

 i<<4; // assigning :f = 4
 i>>s;

 while(!s.eof()){ // while not end-of-data
   s>>f1;
   s>>f2;
   cout<<"f1="<<f1<<", f2=";
   for(int j=0;f2[j]!=0;++j)
     printf("%2x ", f2[j]);
   cout<<endl;
 
}
 // there is no need to explicitly calls s.close() since s's destructor 
 // will take care of closing the stream
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
    "create table test_tab(f1 number, f2 varchar2(40))"
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
