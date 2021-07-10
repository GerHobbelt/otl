#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object


void select()
{ 
 otl_stream s;

 s.set_all_column_types(otl_all_num2str|otl_all_date2str); 
    // map all numeric and date/time output columns to strings for simplicity.

 s.open(100, // buffer size
         "$SQLTables",
         // get a list of all tables in the current database
         db // connect object
        ); 

 otl_column_desc* desc;
 int desc_len;
 desc=s.describe_select(desc_len); 
    // describe the structure of the output columns of the result set.

 int rpc=0;
 while(!s.eof()){
   ++rpc;
   char str[512];
   cout<<"ROW#"<<rpc<<" ";
   for(int col_num=0;col_num<desc_len;++col_num){
     s>>str;
     if(s.is_null())
       cout<<desc[col_num].name<<"=NULL ";
     else
       cout<<desc[col_num].name<<"="<<str<<" ";
   }
   cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2 CLI environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to DB2

  select(); // query the system data dictionary via DB2 CLI functions.

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from DB2

 return 0;

}
