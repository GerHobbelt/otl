#include <iostream>
using namespace std;

#include <stdio.h>

#if !defined(ORA_VER_DEFINED)
#define OTL_ORA8I // Compile OTL 4.0/OCI8i
#endif
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
#define OTL_UNICODE // Enable Unicode support
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
  cout<<"charset_form="<<desc[n].charset_form<<endl;
  cout<<"char_size="<<desc[n].char_size<<endl;
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
    "create table test_tab "
    "(f1 number, f2 varchar2(100), f3 nvarchar2(100), "
    "f4 char(100), f5 nchar(100), "
    "f6 clob, f7 nclob)"
    );  // create table

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
