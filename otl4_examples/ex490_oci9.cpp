#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_ORA_UTF8 // Enable UTF8 OTL for OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_long_string f2(6000); // define long string variable

 otl_stream o(1, // buffer size has to be set to 1 for operations with CLOBs
              "insert into test_tab values(:f1<int>,empty_clob()) "
              "returning f2 into :f2<nclob>",
                 // SQL statement
              db // connect object
             );

 o.set_commit(0); // setting stream "auto-commit" to "off". It is required
                  // when LOB stream mode is used.

 otl_lob_stream lob; // LOB stream for reading/writing unlimited number
                     // of bytes regardless of the buffer size.

 int i,j;

 for(i=1;i<=20;++i){
  o<<i;
  o<<lob; // Initialize otl_lob_stream by writing it
          // into otl_stream.

  for(j=0;j<5000;++j)
   f2[j]='*';
  f2[5000]='?';
  f2.set_len(5001);

  lob.set_len(5001+2123); // setting the total  size of
                          // the CLOB to be written.
  
  lob<<f2; // writing first chunk of the CLOB into lob

  f2[2122]= '?';
  f2.set_len(2123); // setting the size of the second chunk

  lob<<f2; // writing the second chunk of the CLOB into lob
  lob.close(); // closing the otl_lob_stream

 }

 db.commit(); // committing transaction.

}

void select()
{ 
 otl_long_string f2(6000); // define long string variable

 otl_stream i(10, // buffer size 
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 
 float f1=0;
 otl_lob_stream lob; // Stream for reading CLOB

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  cout<<"f1="<<f1<<endl;
  i>>lob; // initializing CLOB stream by reading the CLOB reference 
          // into the otl_lob_stream from the otl_stream.
  int n=0;
  while(!lob.eof()){ // read while not "end-of-file" -- end of CLOB
   ++n;
   lob>>f2; // reading a chunk of CLOB
   cout<<"   chunk #"<<n;
   cout<<", f2="
       <<f2[0]
       <<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
  }
  lob.close(); // closing the otl_lob_stream. This step may be skipped.  
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
    "create table test_tab(f1 number, f2 nclob)"
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
