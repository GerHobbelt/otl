#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
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

void insert()
// insert rows into table
{ 
 otl_stream o;
 o.setBufSize(50);

// Send a message (SQL statement) to the otl_connect object.
 db<<="insert into test_tab values(:f1<int>,:f2<char[31]>)";

// Send a message (SQL statement) from the connect object 
// to the otl_stream object. 

 db>>o;

// By and large, this is all syntactical sugar, but "some like it hot".

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
  o<<i<<tmp;
 }
}

void select()
{ 
 otl_stream i;

 i.setBufSize(50);

// Send a message (SQL statement) to the otl_connect object.
 db<<="select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2";

// Send a message (SQL statement) from the connect object 
// to the otl_stream object. 

 db>>i;

// By and large, this is all syntactical sugar, but "some like it hot".
 
 int f1=0;
 char f2[31];

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 i<<4<<4; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db<<"informix/tigger@informixsql"; // connect to the database

  // Send SQL statements to the connect obejct for immediate execution. 
  // Ignore any exception for the first statement.
  try{ db<<"drop table test_tab"; } catch(otl_exception&){}
  db<<"create table test_tab(f1 int, f2 varchar(30))";

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
