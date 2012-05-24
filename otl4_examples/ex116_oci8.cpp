#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4/OCI8
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void insert1()
// insert rows into table
{ 
 otl_stream o; // define an otl_stream variable

 o.set_flush(false); // set the auto-flush flag to OFF.

o.open(200, // buffer size
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
  if(i%55==0)
   throw "Throwing an exception";
 }
 o.flush(); // when the auto-flush flag is OFF, an explicit flush
            // of the stream buffer is required in case of successful
            // completion of execution of the INSERT statement.
// In case of a raised exception, the stream buffer would not be flushed.           
}

void insert2()
// insert rows into table
{ 
 otl_stream o; // define an otl_stream variable

 o.set_flush(false); // set the auto-flush flag to OFF.

o.open(200, // buffer size
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
//  if(i%55==0)
//   throw "Throwing an exception";
 }
 o.flush(); // when the auto-flush flag is OFF, an explicit flush
            // of the stream buffer is required in case of successful
            // completion of execution of the INSERT statement.
// In case of a raised exception, the stream buffer would not be flushed.           
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1;
 char f2[31];

 i<<8; // assigning :f = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
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
    "create table test_tab(f1 number, f2 varchar2(30))"
    );  // create table

  try{
   insert1(); // insert records into table
  }catch(const char* p){
   cout<<p<<endl;
  }
  cout<<"Selecting the first time around:"<<endl;
  select(); // select records from table
  
  insert2();
  cout<<"Selecting the second time around:"<<endl;
  select();

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
