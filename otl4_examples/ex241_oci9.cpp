#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_ORA_TIMESTAMP // enable Oracle 9i TIMESTAMPs [with [local] time zone]
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert(void)
// insert rows into table
{ 
 otl_stream o(3, // stream buffer size in logical rows.
              "insert into test_tab "
              "values(:f1<int>,:f2<ltz_timestamp>,:f3<tz_timestamp>,:f4<timestamp>)", 
                 // INSERT statement
              db // connect object
             );

 otl_datetime f2, f3, f4;

 for(int i=1;i<=10;++i){
// populating a timestamp with local time zone
  f2.year=1998;
  f2.month=10;
  f2.day=19;
  f2.hour=23;
  f2.minute=12;
  f2.second=12;
  f2.frac_precision=6; // microseconds
  f2.fraction=123456;
// populating a timestamp with time zone
  f3=f2;
  f3.tz_hour=-9;
  f3.tz_minute=0;
// populating a simple timestamp
   f4=f2;
  o<<i;
  o<<f2;
  o<<f3;
  o<<f4;
 }
}

void select(void)
{ 
 otl_stream i(5, // stream buffer size in logical rows
              "select * from test_tab "
              "where f1=:f1<int> "
              "  and f3=:f3<tz_tmestamp>",
                 // SELECT statement
              db // connect object
             ); 
// input parameters for the SELECT statement
 otl_datetime in_f3;

// SELECT output columns
 int f1=0;
 otl_datetime f2, f3, f4;

// setting second fraction precision to 6, microseconds
  f2.frac_precision=6;
  f3.frac_precision=6;
  f4.frac_precision=6;

// populating input "timestamp with time zone"
  in_f3.year=1998;
  in_f3.month=10;
  in_f3.day=19;
  in_f3.hour=23;
  in_f3.minute=12;
  in_f3.second=12;
  in_f3.frac_precision=6; // microseconds
  in_f3.fraction=123456;
  in_f3.tz_hour=-9;
  in_f3.tz_minute=0;

// writing input parameters to the stream
 i<<5<<in_f3;

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2>>f3>>f4;
  cout<<"f1="<<f1<<endl;
  cout<<"f2="<<f2.month<<"/"<<f2.day<<"/"
      <<f2.year<<" "<<f2.hour<<":"<<f2.minute<<":"
      <<f2.second<<"."
      <<f2.fraction<<" "
      <<f2.tz_hour<<":"
      <<f2.tz_minute
      <<endl;
  cout<<"f3="<<f3.month<<"/"<<f3.day<<"/"
      <<f3.year<<" "<<f3.hour<<":"<<f3.minute<<":"
      <<f3.second<<"."
      <<f3.fraction<<" "
      <<f3.tz_hour<<":"
      <<f3.tz_minute
      <<endl;
  cout<<"f4="<<f4.month<<"/"<<f4.day<<"/"
      <<f4.year<<" "<<f4.hour<<":"<<f4.minute<<":"
      <<f4.second<<"."
      <<f4.fraction
      <<endl;
 
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
    "create table test_tab "
    "(f1 int, "
    " f2 timestamp with local time zone, "
    " f3 timestamp with time zone, "
    " f4 timestamp)"
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
