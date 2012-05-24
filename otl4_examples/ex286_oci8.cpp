#include <iostream>
using namespace std;

#include <stdio.h>

#if !defined(ORA_VER_DEFINED)
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
#endif
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
#define OTL_UNICODE // Enable Unicode OTL for OCI8i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_long_unicode_string f2(70000); // define long unicode string variable
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream o(1, // buffer size has to be set to 1 for operations with CLOBs
              "insert into test_tab values(:f1<int>,empty_clob(),empty_clob())"
              "returning f2, f3 into :f2<clob>, :f3<nclob>", 
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=20;++i){
  for(int j=0;j<50000;++j)
   f2[j]='*'; // first 128 characters of Unicode are ASCII
  f2[50000]='?'; // first 128 characters of Unicode are ASCII
  f2.set_len(50001);
  o<<i<<f2<<f2;
 }

}

void select()
{ 
 otl_long_unicode_string f2(70000); // define long string variable
 otl_long_unicode_string f3(70000); // define long string variable
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream i(10, // buffer size 
              "select "
               "  f1, f2, "
               "  f3 :#3<nclob> "
                 // override the default mapping of f3 (from clob)
                 // to nclob
               "from test_tab "
               "where f1>=:f<int> "
               "  and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3;
  cout<<"f1="<<f1;
  cout<<", f2="<<static_cast<char>(f2[0])
      <<static_cast<char>(f2[f2.len()-1])<<", len="<<f2.len();
  cout<<", f3="<<static_cast<char>(f3[0])
      <<static_cast<char>(f3[f3.len()-1])<<", len="<<f3.len();
  cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("scott/tiger"); // connect to Oracle

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 clob, f3 nclob)"
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
