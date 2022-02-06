#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4/OCI8
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{otl_long_string f2(60000); // define long string variable
 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "insert into test_tab values(:f1<int>,empty_clob()) "
              "returning f2 into :f2<clob> ",
                 // SQL statement
              db // connect object
             );
 o.set_commit(0); // setting stream "auto-commit" to "off". It is required
                  // when LOB stream mode is used.

 otl_lob_stream lob; // LOB stream for reading/writing unlimited number
                     // of bytes regardless of the buffer size.

 for(int i=1;i<=20;++i){
  for(int j=0;j<50000;++j)
   f2[j]='*';
  f2[50000]='?';
  f2.set_len(50001);

  o<<i;

  o<<lob; // Initialize otl_lob_stream by writing it
          // into otl_stream. Weird, isn't it?

  lob.set_len(50001+23123); // setting the total  size of
                            // the CLOB to be written.
   // It is required for compatibility
   // with earlier releases of OCI8: OCI8.0.3, OCI8.0.4.
 
  lob<<f2; // writing first chunk of the CLOB into lob

  f2[23122]='?';
  f2.set_len(23123); // setting the size of the second chunk

  lob<<f2; // writing the second chunk of the CLOB into lob
  lob.close(); // closing the otl_lob_stream
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

 long rpc=o.get_rpc(); // get the RPC count
 if(rpc==1){
   lob.set_len(6001*4); // setting the total size of of the CLOB to be written
   for(int i=1;i<=4;++i)
     lob<<f2; // writing chunks of the CLOB into the otl_lob_stream
   lob.close(); // closing the otl_lob_stream
 }else if(rpc==0){
   // nothing to do, closing the LOB stream in the "don't throw
   // otl_exception when size doesn't match" mode
   lob.close(true);
 }

 db.commit(); // committing transaction

}

void select()
{ 
 otl_long_string f2(20000); // define long string variable

 otl_stream i(3, // buffer size. To read CLOBs, it can be set to a size greater than 1
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 otl_lob_stream lob; // Stream for reading CLOB

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  cout<<"f1="<<f1<<endl;
  i>>lob; // initializing CLOB stream by reading the CLOB reference 
          // into the otl_lob_stream from the otl_stream.
  if(f1==4 || f1==8)
    lob.setInitialReadOffset(30000);
  int n=0;
  while(!lob.eof()){ // read while not "end-of-file" -- end of CLOB
   ++n;
   lob>>f2; // reading a chunk of CLOB
   cout<<"   chunk #"<<n;
   cout<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
  }
  lob.close(); // closing the otl_lob_stream. This step may be skipped.
  
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
    "create table test_tab(f1 number, f2 clob)"
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

 db.logoff(); // disconnect from Oracle

 return 0;

}
