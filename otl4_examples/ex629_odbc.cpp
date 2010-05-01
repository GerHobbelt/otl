#include <iostream>
using namespace std;

#include <stdio.h>

// Compile OTL 4.0/ODBC in Windows. Informix CLI uses the  
// regular ODBC header files in Windows
#define OTL_ODBC 

// Uncomment the #defines below in Linux / Unix
//#define OTL_ODBC_UNIX
//#define OTL_INFORMIX_CLI
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void select()
{ 
 otl_stream i(1, // buffer size
              "select * from test_tab",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 otl_column_desc* desc;
 int desc_len;

 desc=i.describe_select(desc_len);

 for(int n=0;n<desc_len;++n){
  cout<<"========== COLUMN #"<<n+1<<" ==========="<<endl;
  cout<<"name="<<desc[n].name<<endl;
  cout<<"dbtype="<<desc[n].dbtype<<endl;
  cout<<"otl_var_dbtype="<<desc[n].otl_var_dbtype<<endl;
  cout<<"dbsize="<<desc[n].dbsize<<endl;
  cout<<"scale="<<desc[n].scale<<endl;
  cout<<"prec="<<desc[n].prec<<endl;
  cout<<"nullok="<<desc[n].nullok<<endl;
 }
 

}

int main()
{
 otl_connect::otl_initialize(); // initialize Informix CLI environment
 try{

  db.rlogon("informix/tigger@informixsql"); // connect to Informix

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30), f3 byte)"
    );  // create table

  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Informix

 return 0;

}
