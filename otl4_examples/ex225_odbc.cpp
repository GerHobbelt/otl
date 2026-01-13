#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
// The following #define is required with MyODBC 3.51.11 and higher
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream
   o(1, // buffer size should be == 1 always on INSERT
     "insert into test_tab values "
     " (:f1<int>,:f2<char[31]>), "
     " (:f12<int>,:f22<char[31]>), "
     " (:f13<int>,:f23<char[31]>), "
     " (:f14<int>,:f24<char[31]>), "
     " (:f15<int>,:f25<char[31]>) ",
     // INSERT statement. Multiple sets of values can be used
     // to work around the lack of the bulk interface
     db // connect object
     );

 // If the number of rows to be inserted is not known in advance,
 // another stream with the same INSERT can be opened
 otl_nocommit_stream 
   o2(1, // buffer size should be == 1 always on INSERT
     "insert into test_tab values "
      " (:f1<int>,:f2<char[31]>) ",
     db // connect object
     );

 char tmp[32];
 int i;
 for(i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<i<<tmp;
 }

 for(i=101;i<=103;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o2<<i<<tmp;
 }
}

void update(const int af1)
// insert rows into table
{ 
 otl_stream o(1, // buffer size should be == 1 always on UPDATE
              "UPDATE test_tab "
              "   SET f2=:f2<char[31]> "
              " WHERE f1=:f1<int>", 
                 // UPDATE statement
              db // connect object
             );
#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
 otl_write_row(o,"Name changed",af1);
 otl_write_row(o,otl_null(),af1+1); // set f2 to NULL
#else
  // when variadic template functions are not supported by the C++
  // compiler, OTL provides nonvariadic versions of the same template
  // functions in the range of [1..15] parameters
 const char* name_changed="Name changed";
 otl_write_row(o,name_changed,af1);
 otl_write_row(o,otl_null(),af1+1); // set f2 to NULL
  // the old way (operators >>() / <<()) is available as always:
  //  o<<"Name changed"<<af1;
  //  o<<otl_null()<<af1+1; // set f2 to NULL
#endif

}

void select(const int af1)
{ 
 otl_stream i(50, // buffer size may be > 1
              "select * from test_tab "
              "where f1>=:f11<int> "
              "  and f1<=:f12<int>*2",
                // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;
 char f2[31];

#if defined(OTL_ANSI_CPP_11_VARIADIC_TEMPLATES)
 otl_write_row(i,af1,af1); // :f11 = af1, :f12 = af1
#else
  // when variadic template functions are not supported by the C++
  // compiler, OTL provides nonvariadic versions of the same template
  // functions in the range of [1..15] parameters
 otl_write_row(i,af1,af1); // :f11 = af1, :f12 = af1
  // the old way (operators >>() / <<()) is available as always:
  // i<<af1<<af1; // :f11 = af1, :f12 = af1
#endif

#if (defined(_MSC_VER) && _MSC_VER>=1600) || defined(OTL_CPP_11_ON)
// C++11 (or higher) compiler
 for(auto& it : i){
  it>>f1;
  cout<<"f1="<<f1<<", f2=";
  it>>f2;
  if(it.is_null())
   cout<<"NULL";
  else
   cout<<f2;
  cout<<endl;
 }
#else
// C++98/03 compiler
 for(auto& it : i){ // while not end-of-data
  it>>f1;
  cout<<"f1="<<f1<<", f2=";
  it>>f2;
  if(it.is_null())
   cout<<"NULL";
  else
   cout<<f2;
  cout<<endl;
 }
#endif

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=mysql35"); // connect to ODBC
//  db.rlogon("scott/tiger@mysql35"); // connect to ODBC, alternative format
                                    // of connect string 

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  insert(); // insert records into the table
  update(10); // update records in the table
  select(8); // select records from the table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
