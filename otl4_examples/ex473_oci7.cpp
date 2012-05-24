#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA7 // Compile OTL 4.0/OCI7
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
#define OTL_ORA7_TIMESTAMP_TO_STRING(tm,s)      \
{                                               \
  sprintf_s(s,sizeof(s),                        \
          "%02d/%02d/%04d %02d:%02d:%02d.%06ld",\
          tm.month,                             \
          tm.day,                               \
          tm.year,                              \
          tm.hour,                              \
          tm.minute,                            \
          tm.second,                            \
          tm.fraction                           \
         );                                     \
}

#define OTL_ORA7_STRING_TO_TIMESTAMP(s,tm)       \
{                                                \
  sscanf_s(s,                                    \
          "%02d/%02d/%04d %02d:%02d:%02d.%06ld", \
          &tm.month,                             \
          &tm.day,                               \
          &tm.year,                              \
          &tm.hour,                              \
          &tm.minute,                            \
          &tm.second,                            \
          &tm.fraction                           \
         );                                      \
}
#else
#define OTL_ORA7_TIMESTAMP_TO_STRING(tm,s)      \
{                                               \
  sprintf(s,                                    \
          "%02d/%02d/%04d %02d:%02d:%02d.%06ld",\
          tm.month,                             \
          tm.day,                               \
          tm.year,                              \
          tm.hour,                              \
          tm.minute,                            \
          tm.second,                            \
          tm.fraction                           \
         );                                     \
}

#define OTL_ORA7_STRING_TO_TIMESTAMP(s,tm)       \
{                                                \
  sscanf(s,                                      \
          "%02d/%02d/%04d %02d:%02d:%02d.%06ld", \
          &tm.month,                             \
          &tm.day,                               \
          &tm.year,                              \
          &tm.hour,                              \
          &tm.minute,                            \
          &tm.second,                            \
          &tm.fraction                           \
         );                                      \
}
#endif
#else
#define OTL_ORA7_TIMESTAMP_TO_STRING(tm,s)      \
{                                               \
  sprintf(s,                                    \
          "%02d/%02d/%04d %02d:%02d:%02d.%06ld",\
          tm.month,                             \
          tm.day,                               \
          tm.year,                              \
          tm.hour,                              \
          tm.minute,                            \
          tm.second,                            \
          tm.fraction                           \
         );                                     \
}

#define OTL_ORA7_STRING_TO_TIMESTAMP(s,tm)       \
{                                                \
  sscanf(s,                                      \
          "%02d/%02d/%04d %02d:%02d:%02d.%06ld", \
          &tm.month,                             \
          &tm.day,                               \
          &tm.year,                              \
          &tm.hour,                              \
          &tm.minute,                            \
          &tm.second,                            \
          &tm.fraction                           \
         );                                      \
}
#endif

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab2 "
              "values(:f1<int>, "
               "      to_timestamp(:f2<char[28]>,'MM/DD/YYYY HH24:MI:SS.FF'))", 
                 // SQL statement
              db // connect object
             );

 otl_datetime tm;

 for(int i=1;i<=10;++i){
  tm.year=2007;
  tm.month=1;
  tm.day=19;
  tm.hour=23;
  tm.minute=12;
  tm.second=12;
 // Second's precision needs to be specified BEFORE the stucture can be
 // used either for writing or reading timestamp values, which have second's 
 // fractional parts.
  tm.frac_precision=6;
  tm.fraction=123456;
  o<<i;
  o<<tm; // otl_datetime can be written into a 
         // char[XXX] variable when appropriate 
         // conversion is #defined
 }
}

void select(void)
{ 
 otl_stream i(50, // buffer size
              "select "
              "  f1, "
              "  to_char(f2,'MM/DD/YYYY HH24:MI:SS.FF') f2 "
               "from test_tab2 where f1 between :f1<int> and :f1*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 otl_datetime f2;

 i<<4; // assigning :f1 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

 // Second's precision needs to be specified BEFORE the stucture can be
 // used either for writing or reading timestamp values, which have second's 
 // fractional parts.
 f2.frac_precision=6; // microseconds

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  i>>f2; // otl_datetime can be read from a 
         // char[XXX] variable when appropriate 
         // conversion is #defined
  cout<<"f1="<<f1<<", f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second<<"."
      <<f2.fraction
      <<endl;
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
    "drop table test_tab2",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab2(f1 int, f2 timestamp)"
    );  // create table

  insert(); // insert records into table
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
