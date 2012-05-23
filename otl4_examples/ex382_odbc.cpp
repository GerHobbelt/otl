#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
#define OTL_STL // enable OTL support for STL
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{
 string f2; 
 otl_nocommit_stream o; // defined an otl_stream variable
 o.set_lob_stream_mode(true); // set the "lob stream mode" flag
 o.open(1, // buffer size has to be set to 1 for operations with LOBs
        "insert into test_tab values(:f1<int>,:f2<varchar_long>) ",
            // SQL statement
        db // connect object
       );
 otl_lob_stream lob; // LOB stream for reading/writing unlimited number
                     // of bytes regardless of the buffer size.
 for(int i=1;i<=20;++i){
  f2.erase();
  f2.append(50000,'*');
  f2 += '?';
  o<<i;
  o<<lob; // Initialize otl_lob_stream by writing it
          // into otl_stream

  lob.set_len(50001+23123); // setting the total size of
                            // the LONGTEXT to be written.
   // It is required for bakward compatibility
 
  lob<<f2; // writing first chunk of the LONGTEXT into lob

  f2.erase();
  f2.append(23122,'*');
  f2 += '?';
  lob<<f2; // writing the second chunk of the LONGTEXT into lob
  lob.close(); // closing the otl_lob_stream
 }

 db.commit(); // committing transaction.
}

void update()
// insert rows in table
{ 
 string f2; 
 otl_stream o; // defined an otl_stream variable
 o.set_lob_stream_mode(true); // set the "lob stream mode" flag
 o.open(1, // buffer size has to be set to 1 for operations with LOBs
        "update test_tab "
        "   set f2=:f2<varchar_long> "
        "where f1=:f1<int> ",
            // SQL statement
        db // connect object
       );
 otl_lob_stream lob;
 o.set_commit(0); // setting stream "auto-commit" to "off". 
 f2.append(6000,'#');
 f2+='?';
 o<<lob; // Initialize otl_lob_stream by writing it
         // into otl_stream.
 o<<5;
 lob.set_len(6001*4); // setting the total size of of the LONGTEXT to be written
 for(int i=1;i<=4;++i)
  lob<<f2; // writing chunks of the LONGTEXT into the otl_lob_stream
 lob.close(); // closing the otl_lob_stream
 db.commit(); // committing transaction

}

void select()
{ 
 string f2;
 otl_stream i; // defined an otl_stream variable
 i.set_lob_stream_mode(true); // set the "lob stream mode" flag
 i.open(1, // buffer size. To read LONGTEXTs, it should be set to 1
        "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
          // SELECT statement
        db // connect object
       ); 
   // create select stream
 
 int f1;
 otl_lob_stream lob; // Stream for reading LONGTEXT

 lob.setStringBuffer(40000); // Set a bigger internal buffer (default is 4096 bytes)
                             // for reading chunks of the LONGTEXT, before each chunk
                             // gets appended to the output string.
 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  cout<<"f1="<<f1;
  i>>lob; // initializing LONGTEXT stream by reading the LONGTEXT reference 
          // into the otl_lob_stream from the otl_stream.
  lob>>f2;
  cout<<", f2="<<f2[0]<<f2[f2.length()-1]
      <<", len="<<static_cast<int>(f2.length())<<endl;
  lob.close(); // closing the otl_lob_stream. This step may be skipped.
  
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@mysql35"); // connect to the database

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
  update(); // update records in table
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
