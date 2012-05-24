#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#if defined(__BORLANDC__)
#define OTL_BIGINT __int64 // Enabling G++ 64-bit integers
#elif !defined(_MSC_VER)
#define OTL_BIGINT long long // Enabling G++ 64-bit integers
#else
#define OTL_BIGINT __int64 // Enabling VC++ 64-bit integers
#endif
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<bigint>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(OTL_BIGINT i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
   sprintf_s(tmp,sizeof(tmp),"Name%d",static_cast<int>(i));
#else
   sprintf(tmp,"Name%d",static_cast<int>(i));
#endif
#else
   sprintf(tmp,"Name%d",static_cast<int>(i));
#endif
  o<<i<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<bigint> and f1<=:ff<bigint>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 OTL_BIGINT f1;
 char f2[31];
#if defined(_MSC_VER)
 char f1str[40];
#endif

 i<<static_cast<OTL_BIGINT>(8)
  <<static_cast<OTL_BIGINT>(8); // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1=";
#if !defined(_MSC_VER)
  cout<<f1;
#else
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  _i64toa_s(f1,f1str,sizeof(f1str),10);
#else
  _i64toa(f1,f1str,10);
#endif
#else
  _i64toa(f1,f1str,10);
#endif
  cout<<f1str;
#endif
  cout<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2 CLI environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to DB2

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 bigint, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

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
