#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#define OTL_STL
#define OTL_ORA9I // Compile OTL 4.0/OCI9I
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
//#define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10gR2
//#define OTL_ORA11G // Compile OTL 4.0/OCI11G
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 db.set_max_long_size(80000); // set maximum long string size for connect object
 otl_stream o(1, // buffer size needs to be set to 1 when varchar_long is used
              "insert into test_tab values(:f1<int>,:f2<varchar_long>) ",
                 // SQL statement
              db // connect object
             );
 char tmp[80001];

 for(int i=1;i<=20;++i){
  sprintf(tmp,"<TAG>MyXML%d</TAG>",i);
  string tmp_str(tmp);
  o<<i<<tmp_str;
 }

}

void select()
{ 
 string f2;
 db.set_max_long_size(80000); // set maximum long string size for connect object

 otl_stream i(10, // buffer size. To read XML as CLOBs, it can be set to a size greater than 1
              "select f1, to_clob(f2) "
              "from test_tab "
              "where f1>=:f11<int> "
              " and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8 
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 
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
    "create table test_tab(f1 number, f2 xmltype)"
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
