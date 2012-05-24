#include <iostream>
using namespace std;
#include <stdio.h>
#include <windows.h>
#include <process.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
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

otl_stream s;

void select(void* dummy_par)
{ 
 try{

  cout<<"Selecting..."<<endl;

  s.open(1, // buffer size
         "select count(*) "
         "from test_tab a1, test_tab a2, test_tab a3, "
         "     test_tab a4, test_tab a5 "
         "where 1=:f1<int>",
           // SELECT statement
         db // connect object
        ); 
   // Create select stream.
   // The ":f1" dummy input parameter is needed to give the stream
   // a chance to initialize its internal variables with the SELECT statement
   // BEFORE the SELECT statement starts executing
 
  int count;

  s<<1; // Now, forcing the execution of the SQL

  while(!s.eof()){ // while not end-of-data
   s>>count;
   cout<<"count="<<count<<endl;
  }
 }catch(otl_exception& p){
  cerr<<"Select() ===> "<<p.msg<<endl; // print out error message
  cerr<<"Select() ===> "<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<"Select() ===> "<<p.var_info<<endl; // print out the vari }
 }
 _endthread();
}

void cancel(void* dummy_par)
{ 
 try{
   cout<<"Cancelling the SQL"<<endl;
   s.cancel();
 }catch(otl_exception& p){
   cout<<"Cancel() ===> "<<p.msg<<endl; // print out error message
   cout<<"Cancel() ===> "<<p.stm_text<<endl; // print out SQL that caused the error
   cout<<"Cancel() ===> "<<p.var_info<<endl; // print out the vari }
 }
 cout<<"End of Cancelling"<<endl;
 _endthread();

}

int main()
{
 otl_connect::otl_initialize(); // initialize the environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to the database

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

  insert(); // insert records into table
  _beginthread(select,0,NULL);
  Sleep(2000);
  _beginthread(cancel,0,NULL);
  Sleep(10000);
  cout<<"End of program"<<endl;
 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
