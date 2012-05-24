#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA10G // Compile OTL 4.0/OCI

// This #define overrides the default (any NUMBER to double) numeric data type 
// mapping on SELECT statements / reference cursors. 
// This #define maps NUMBER(X) (X in [1..4]) to short int, NUMBER(Y) 
// (Y in [5..10]) to int, any other NUMBER to double. 
#define OTL_ORA_CUSTOM_MAP_NUMBER_ON_SELECT(ftype,elem_size,desc)      \
{                                                                       \
  if(ftype==extFloat && desc.prec>=5 && desc.prec<=10 && desc.scale==0){ \
    ftype=otl_var_int;                                                  \
    elem_size=sizeof(int);                                              \
  }else if(ftype==extFloat && desc.prec>=1 && desc.prec<=4 && desc.scale==0){ \
    ftype=otl_var_short;                                                \
    elem_size=sizeof(short int);                                        \
  }else{                                                                \
    ftype=otl_var_double;                                               \
    elem_size=sizeof(double);                                           \
  }                                                                     \
}
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void select()
{ 
 otl_stream i(1, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
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
    "create table test_tab(f1 number, f2 number(4), f3 number(10))"
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
