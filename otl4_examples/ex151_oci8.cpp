#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8i
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{otl_long_string f2(6000); // define long string variable
 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "insert into test_tab values(:f1<int>,empty_clob(),empty_clob()) "
              "returning f2, f3 into :f2<clob>,:f3<clob> ",
                 // SQL statement
              db // connect object
             );
 o.set_commit(0); // setting stream "auto-commit" to "off". It is required
                  // when LOB stream mode is used.

 int i,j;
 otl_lob_stream lob; // LOB stream for reading/writing unlimited number
                     // of bytes regardless of the buffer size.
 otl_lob_stream lob2; // LOB stream for reading/writing unlimited number
                     // of bytes regardless of the buffer size.

 for(i=1;i<=20;++i){
  o<<i;
  o<<lob; // Initialize otl_lob_stream by writing it
          // into otl_stream.
  o<<lob2; // Initialize otl_lob_stream by writing it
          // into otl_stream.

  for(j=0;j<5000;++j)
   f2[j]='*';
  f2[5000]='?';
  f2.set_len(5001);

  lob.set_len(5001+2123); // setting the total  size of
                          // the CLOB to be written.
  
  lob<<f2; // writing first chunk of the CLOB into lob


  f2[2122]='?';
  f2.set_len(2123); // setting the size of the second chunk

  lob<<f2; // writing the second chunk of the CLOB into lob
  lob.close(); // closing the otl_lob_stream

  for(j=0;j<5000;++j)
   f2[j]='*';
  f2[5000]='?';
  f2.set_len(5001);
  lob2.set_len(5001+2123); // setting the total  size of
                          // the CLOB to be written.
  
  lob2<<f2; // writing first chunk of the CLOB into lob

  f2[2122]='?';
  f2.set_len(2123); // setting the size of the second chunk

  lob2<<f2; // writing the second chunk of the CLOB into lob
  lob2.close(); // closing the otl_lob_stream

 }

 db.commit(); // committing transaction.
}

void update()
// insert rows in table
{

 otl_long_string f2(6200); // define long string variable

 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "update test_tab "
              "   set f2=empty_clob() "
              "where f1=:f1<int> "
              "returning f2 into :f2<clob> ",
                 // SQL statement
              db // connect object
             );

  otl_lob_stream lob;

  o.set_commit(0); // setting stream "auto-commit" to "off". 


 for(int j=0;j<6000;++j){
  f2[j]='#';
 }

 f2[6000]='?';
 f2.set_len(6001);

 o<<5;
 o<<lob; // Initialize otl_lob_stream by writing it
         // into otl_stream.

 lob.set_len(6001*4); // setting the total size of of the CLOB to be written
 for(int i=1;i<=4;++i)
  lob<<f2; // writing chunks of the CLOB into the otl_lob_stream

 lob.close(); // closing the otl_lob_stream

 db.commit(); // committing transaction

}

void select()
{ 
 otl_long_string f2(3000); // define long string variable

 otl_stream i(1, // buffer size
              "BEGIN "
              "  open :cur<refcur,out[10]> for "
     // :cur is a bind variable name, refcur -- its type, 
     // out -- output parameter, 10 -- the buffer size when this
     // reference cursor will be attached to otl_refcur_stream 
              "  select * from test_tab "
              "  where f1>=:f11<int,in> "
              "    and f1<=:f12<int,in>*2; "
              "END;", // PL/SQL block, returning a reference cursor
              db // connect object
             ); 
  i.set_commit(0); // set stream's "auto-commit" to OFF
 
 int f1=0;
 otl_lob_stream lob, lob2; // Streams for reading CLOBs
 otl_refcur_stream s; // reference cursor stream for reading rows.

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer
 i>>s;

 while(!s.eof()){ // while not end-of-data
  s>>f1;
  cout<<"f1="<<f1<<endl;
  s>>lob; // initializing CLOB stream by reading the CLOB reference 
          // into the otl_lob_stream from the otl_refcur_stream.
  int n=0;
  while(!lob.eof()){ // read while not "end-of-file" -- end of CLOB
   ++n;
   lob>>f2; // reading a chunk of CLOB
   cout<<"   chunk #"<<n;
   cout<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
  }
  lob.close(); // closing the otl_lob_stream.

  s>>lob2; // initializing CLOB stream by reading the CLOB reference 
          // into the otl_lob_stream from the otl_refcur_stream.
  n=0;
  while(!lob2.eof()){ // read while not "end-of-file" -- end of CLOB
   ++n;
   lob2>>f2; // reading a chunk of CLOB
   cout<<"   chunk #"<<n;
   cout<<", f3="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
  }
  lob2.close(); // closing the otl_lob_stream.
 }
 s.close(); // closing the reference cursor stream

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
    "create table test_tab(f1 number, f2 clob, f3 clob)"
    );  // create table

  insert(); // insert records into table
  update(); // update table
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
