#include <iostream>
using namespace std;
#include <stdio.h>

//#define OTL_ORA8I // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8i
#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>,:f3<int>)", 
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
  o<<i<<tmp<<i;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<char[20]>, f2, f3 :#3<short> "
  	      "from test_tab "
  	      "where f1>=:f11<int> "
    	      "  and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 char f1[20];
 char f2[31];
 short int f3;

 i<<4<<4; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3;
  cout<<"f1='"<<f1<<"', f2="<<f2<<", f3="<<f3<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the databse API environment
 try{

  db.rlogon("scott/tiger"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30), f3 int)"
    );  // create table

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
