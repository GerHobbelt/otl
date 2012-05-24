#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI8i
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
//#define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10g Release 2
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{otl_long_string f2(5000); // define long string variable
 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "begin "
              "  insert_proc(:f1<int,in>,:f2<clob,in>); "
              "end;", // PL/SQL block that calls a stored procedure
              db // connect object
             );
 o.set_commit(0); // setting stream "auto-commit" to "off". It is required
                  // when LOB stream mode is used.

 otl_lob_stream lob; // LOB stream for reading/writing unlimited number
                     // of bytes regardless of the buffer size.

 for(int i=1; i<=3; ++i){
   for(int j=0;j<4000;++j)
     f2[j]='*';
   f2[4000]='?';
   f2.set_len(4001);
   o<<i;
   o<<lob; // Initialize otl_lob_stream by writing it into the otl_stream
   lob.set_len(4001); // setting the total size of of the CLOB to be written
   lob<<f2; // writing f2 into lob
   lob.close(); // closing the otl_lob_stream
 }
 db.commit(); // committing transaction.
}

void update()
// insert rows in table
{ 

 otl_long_string f2(5200); // define long string variable

 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "begin "
              "  update_proc(:f1<int,in>,:f2<clob,in>); "
              "end;", // PL/SQL block that calls a stored procedure
              db // connect object
             );

  otl_lob_stream lob;

  o.set_commit(0); // setting stream "auto-commit" to "off". 


 for(int j=0;j<5000;++j){
  f2[j]='#';
 }
 f2[5000]='?';
 f2.set_len(5001);
 o<<2;
 o<<lob; // Initialize otl_lob_stream by writing it
         // into otl_stream.
 lob.set_len(5001*4); // setting the total size of of the CLOB to be written
 for(int i=1;i<=4;++i)
   lob<<f2; // writing chunks of the CLOB into the otl_lob_stream
 lob.close(); // closing the otl_lob_stream

 db.commit(); // committing transaction

}

void select(int f1)
{ 
 otl_long_string f2(6000); // define long string variable

 otl_stream i(1, // buffer size needs to be set to 1
              "begin "
              "  select_proc(:f1<int,in>,:f2<clob,out>); "
              "end;", // PL/SQL block that calls a stored procedure
              db // connect object
             ); 
 
 otl_lob_stream lob; // Stream for reading CLOB

 i<<f1;
 cout<<"f1="<<f1<<endl;
 i>>lob; // initializing CLOB stream by reading the CLOB reference 
         // into the otl_lob_stream from the otl_stream.
 int n=0;
 while(!lob.eof()){ // read while not "end-of-file" -- end of CLOB
   ++n;
   lob>>f2; // reading a chunk of CLOB
   cout<<"   chunk #"<<n;
   cout<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
  }
 lob.close(); // closing the otl_lob_stream. This step may be skipped.

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db<<"scott/tiger"; // connect to the database

  // Send SQL statements to the connect obejct for immediate execution. 
  // Ignore any exception for the first statement.
  try{ db<<"drop table test_tab"; } catch(otl_exception&){}
  db<<"create table test_tab(f1 int, f2 clob)";
  db<<"CREATE OR REPLACE PROCEDURE insert_proc "
      "  (A in NUMBER, "
      "   B in out nocopy CLOB) "
      "IS "
      "BEGIN "
      "  insert into TEST_TAB (F1, F2) "
      "  values (a, EMPTY_CLOB()) "
      "  returning F2 into B; "
      "END;";
  db<<"CREATE OR REPLACE PROCEDURE update_proc "
      "  (A in NUMBER, "
      "   B in out nocopy CLOB) "
      "IS "
      "BEGIN "
      "  select F2 "
      "    into B "
      "    from TEST_TAB "
      "   where F1 = A "
      "  for update nowait; "
      "END;";
  db<<"CREATE OR REPLACE PROCEDURE select_proc "
      "  (A in NUMBER, "
      "   B in out nocopy CLOB) "
      "IS "
      "BEGIN "
      "  select F2 "
      "    into B "
      "  from TEST_TAB "
      "  where f1 = A; "
    "END;";
  insert(); // insert records into table
  update(); // update records in table
  select(1); // select records from table
  select(2); // select records from table
  select(3); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
