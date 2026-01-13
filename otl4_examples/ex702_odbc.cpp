#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
// The following #define is required with MyODBC 5.1 and higher
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#define OTL_UNICODE // Compile OTL with Unicode
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_long_unicode_string f2(6000); // define long unicode string variable

 otl_stream o;

 o.set_lob_stream_mode(true); // set the "lob stream mode" flag
 o.open(1, // buffer size has to be set to 1 for operations with LONGTEXTs
        "insert into test_tab values(:f1<int>,:f2<varchar_long>)",
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
   f2[j]='*'; // ASCII to Unicode.
  f2[5000]='?'; // ASCII to Unicode
  f2.set_len(5001);

  lob.set_len(5001+2123); // setting the total  size of
                          // the LONGTEXTS to be written.
  
  lob<<f2; // writing first chunk of the LONGTEXT into lob

  f2[2122]= '?';
  f2.set_len(2123); // setting the size of the second chunk

  lob<<f2; // writing the second chunk of the LONGTEXT into lob
  lob.close(); // closing the otl_lob_stream

 }

 db.commit(); // committing transaction.

}

void select()
{ 
 otl_long_unicode_string f2(6000); // define Unicode long string variable

 otl_stream i;
 i.set_lob_stream_mode(true); // set the "lob stream mode" flag
 i.open(1, // buffer size needs to be set to 1 in this case
        "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
           // SELECT statement
        db // connect object
       ); 
   // create select stream
 
 
 int f1=0;
 otl_lob_stream lob; // Stream for reading LONGTEXT

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1;
  cout<<"f1="<<f1<<endl;
  it>>lob; // initializing LONGTEXT stream by reading the LONGTEXT reference 
          // into the otl_lob_stream from the otl_stream.
  int n=0;
  while(!lob.eof()){ // read while not "end-of-file"
 // while not end-of-data
 // read while not "end-of-file" -- end of LONGTEXT
   ++n;
   lob>>f2; // reading a chunk of LONGTEXT
   cout<<"   chunk #"<<n;
   cout<<", f2="
       <<static_cast<char>(f2[0]) // Unicode to ASCII
       <<static_cast<char>(f2[f2.len()-1])<<", len="<<f2.len()<<endl;
  

}
  lob.close(); // closing the otl_lob_stream. This step may be skipped.  
 
}
}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@mysql51"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 longtext)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
