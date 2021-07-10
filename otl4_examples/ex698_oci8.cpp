#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#if !defined(OTL_ORA11G_R2)
#define OTL_ORA11G_R2 // Compile OTL 4.0/OCI11R2
#endif
#if defined(__BORLANDC__)
#define OTL_BIGINT __int64 // Enabling G++ 64-bit integers
#define OTL_UBIGINT unsigned __int64 // Enabling G++ 64-bit integers
#elif !defined(_MSC_VER)
#define OTL_BIGINT long long // Enabling G++ 64-bit integers
#define OTL_UBIGINT unsigned long long // Enabling G++ 64-bit integers
#else
#define OTL_BIGINT __int64 // Enabling VC++ 64-bit integers
#define OTL_UBIGINT unsigned __int64 // Enabling VC++ 64-bit integers
#endif
#include <otlv4.h> // include the OTL 4.0 header file

const OTL_UBIGINT UBIG_VAL1=18446744073709551615ULL;

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<bigint>,:f2<char[31]>,:f3<ubigint>)", 
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
   o<<i<<tmp<<UBIG_VAL1;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<bigint>, f2, f3 :#3<ubigint> "
              "from test_tab "
              "where f1>=:f<bigint> and f1<=:ff<bigint>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 OTL_BIGINT f1=0;
 char f2[31]={0};
 OTL_UBIGINT f3=0;

 i<<static_cast<OTL_BIGINT>(8)
  <<static_cast<OTL_BIGINT>(8); // assigning :f = 8; :ff = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
   i>>f1>>f2>>f3;
   printf("f1=%lld, f2=%s, f3=%llu\n",f1,f2,f3);
 }

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
    "create table test_tab(f1 number, f2 varchar2(30), f3 number)"
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

 db.logoff(); // disconnect from Oracle

 return 0;

}
