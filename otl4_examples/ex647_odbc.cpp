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

// Default "numeric" NULLs to (-1)
#define OTL_DEFAULT_NUMERIC_NULL_TO_VAL (-1)

// Default "datetime" NULLs to otl_datetime(2000,1,1,0,0,0)
#define OTL_DEFAULT_DATETIME_NULL_TO_VAL otl_datetime(2000,1,1,0,0,0)

// Default "string" NULLs to "***NULL***"
#define OTL_DEFAULT_STRING_NULL_TO_VAL "***NULL***"

// Default "char" NULLs to '*'
#define OTL_DEFAULT_CHAR_NULL_TO_VAL '*'

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab "
              "values(:f1<int>,:f2<char[31]>,NULL,:f4<timestamp>)",  
                 // SQL statement
              db // connect object
             );
 char tmp[32];
 otl_datetime f4(2001,2,24,13,21,11);

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
  if(i==10)
   o<<i<<otl_null()<<otl_null();
  else
   o<<i<<tmp<<f4;
 }

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1,f2,f2 f22,f3,f4 "
               "from test_tab "
               "where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1=0;
 char f2[31];
 char f22=0;
 int f3=0;
 otl_datetime f4;


 
 i<<8<<8; // assigning :f11 = 8, :f12=8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 cout<<"=====> Fetching rows and printing out default values instead of NULLs"<<endl;
 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1;
  cout<<"f1="<<f1<<", ";
  it>>f2;
  cout<<"f2="<<f2<<", ";
  it>>f22;
  cout<<"f22="<<f22<<", ";
  it>>f3;
  cout<<"f3="<<f3<<", ";
  it>>f4;
  cout<<"f4="<<f4.month<<"/"
      <<f4.day<<"/"
      <<f4.year<<" "
      <<f4.hour<<":"
      <<f4.minute<<":"
      <<f4.second;
  cout<<endl;
 
}

 i<<8<<8; // assigning :f11 = 8, :f12=8
 cout<<"=====> Fetching rows and printing out NULLs"<<endl;
 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1;
  if(it.is_null())
   cout<<"f1=NULL, ";
  else
   cout<<"f1="<<f1<<", ";
  it>>f2;
  if(it.is_null())
   cout<<"f2=NULL, ";
  else
   cout<<"f2="<<f2<<", ";
  it>>f22;
  if(it.is_null())
   cout<<"f22=NULL, ";
  else
   cout<<"f22="<<f22<<", ";
  it>>f3;
  if(it.is_null())
   cout<<"f3=NULL, ";
  else
   cout<<"f3="<<f3<<", ";
  it>>f4;
  if(it.is_null())
   cout<<"f4=NULL,";
  else
   cout<<"f4="<<f4.month<<"/"
       <<f4.day<<"/"
       <<f4.year<<" "
       <<f4.hour<<":"
       <<f4.minute<<":"
       <<f4.second;
  cout<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize Informix CLI environment
 try{

  db.rlogon("informix/tiger@informixsql"); // connect to Informix

  otl_cursor::direct_exec
   (db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (db,
    "create table test_tab(f1 int, f2 varchar(30), f3 int, f4 datetime year to second)"
    );  // create table

  insert(); // insert records into table
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
