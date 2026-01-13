#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>
#include <windows.h>
#include <process.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8(i)
// #define OTL_ORA9I // Compile OTL 4.0/OCI9(i)
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<static_cast<float>(i)<<tmp;
 }
}

void select(void* /*dummy_par*/)
{ 
 try{
  otl_stream i;

  i.open(50, // buffer size
         "select count(*) "
         "from test_tab a1, test_tab a2, test_tab a3, "
         "     test_tab a4, test_tab a5",
           // SELECT statement
         db // connect object
        ); 
   // create select stream
 
  int count;

  for(auto& it : i){ // while not end-of-data
 // while not end-of-data
   it>>count;
   cout<<"count="<<count<<endl;
  
}
 }catch(otl_exception& p){
  cerr<<"Select() ===> "<<p.msg<<endl; // print out error message
  cerr<<"Select() ===> "<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<"Select() ===> "<<p.var_info<<endl; // print out the vari }
 }
 _endthread();
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
    "create table test_tab(f1 number, f2 varchar2(30))"
    );  // create table

  insert(); // insert records into table
  _beginthread(select,0,NULL);
  Sleep(1000);
  cout<<"Cancel the SQL"<<endl;
  db.cancel();
  cout<<"Done"<<endl;
  Sleep(1000);

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
