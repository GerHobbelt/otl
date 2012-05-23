/*
[i_a] ripped example 675 and dropped in my own specials: the TIMESTAMP_TO_STR et al custom defines.

For now, it's a compile-time only example, as it hasn't been tested @ runtime.
*/

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>






//-----------------------------------------------------
//-----------------------------------------------------
// [i_a] customization start; copy&paste of own otl_include_0.h + few tweaks

//#include <ostream>

// OTLv4 config:
//#undef OTL_INCLUDE_0
//#define OTL_INCLUDE_0 1

#define OTL_DESTRUCTORS_DO_NOT_THROW  1
#define OTL_EXCEPTION_ENABLE_ERROR_OFFSET 1
#define OTL_EXTENDED_EXCEPTION 1

#define OTL_ODBC_TIME_ZONE // enable otl_datime's tz components for ODBC

#define OTL_ODBC_MULTI_MODE // Compile OTL 4.0/ODBC multi-mode
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used

// #define OTL_ODBC_MSSQL_2005 1

#define OTL_DEFAULT_STRING_NULL_TO_VAL ""
// #define OTL_ODBC_TIME_ZONE

#define OTL_ODBC_POSTGRESQL 1

#define OTL_ODBC_MYSQL 1

#define OTL_STL                         1

#define OTL_TRACE_ENABLE_STREAM_LABELS                  1

#define OTL_FUNC_THROW_SPEC_ON                                  1

#define OTL_UNCAUGHT_EXCEPTION_ON                               1

#define OTL_VALUE_TEMPLATE_ON                                   1
#define OTL_VALUE_TEMPLATE

#define OTL_BIND_VAR_STRICT_TYPE_CHECKING_ON    1

#define OTL_DEFAULT_DATETIME_NULL_TO_VAL                otl_datetime(0, 0, 0, 0, 0, 0, 0, 0)

#define OTL_ODBC_STRING_TO_TIMESTAMP(str, tm)    otl_str_to_tm(str, tm)
#define OTL_ODBC_TIMESTAMP_TO_STRING(tm, str)    otl_ts_to_str(tm, str)

#define OTL_THROWS_ON_SQL_SUCCESS_WITH_INFO                     1

#define OTL_TRACE_LEVEL                         otlv4_trace_global
#define OTL_TRACE_STREAM                        get_otlv4_trace_stream()

/*
Be warned:

we need to provide forward references for these, but OTL defines 

  STD_NAMESPACE_PREFIX

in the otlv4.h header file only when these configuration settings have been grokked, so
we don't have that one around here. Nevertheless, unless we fall back to 

  using namespace std;

defaulting (which you may not want for some valid reason), you'd better make sure
OTL's guess for STD_NAMESPACE_PREFIX equals the namespace you're actually using here.

It would have been 'better' if it hadn't been named 'STD_NAMESPACE_PREFIX' but 
'OTL_STD_NAMESPACE_PREFIX' plus the additional preproc logic

#if !defined(OTL_STD_NAMESPACE_PREFIX)
#define OTL_STD_NAMESPACE_PREFIX   ...::
#endif

so we could put it here, so we don't have to doublecheck with otlv4.
*/
#define OTL_STD_NAMESPACE_PREFIX     std::    // not used right now

class otl_datetime;

extern int otlv4_trace_global;


extern std::ostream &get_otlv4_trace_stream(void);

extern void otl_str_to_tm(const char *str, otl_datetime &tm);
extern void otl_ts_to_str(const otl_datetime &tm, char str[]);

// MSVC is not 100% C99:
#if defined(_MSC_VER)
typedef __int64   int64_t;
#endif

#define OTL_BIGINT int64_t


//#define OTL_TIMESTAMP_TO_OSTREAM(o, ts)        otl_timestamp_to_ostream(o, ts)

extern std::ostream& otl_timestamp_to_ostream(std::ostream& dst, const otl_datetime &src);


// [i_a] customization ends
//-----------------------------------------------------
//-----------------------------------------------------




#include <otlv4.h> // include the OTL 4.0 header file





otl_connect db; // connect object








// should be switchable through argv[] commandline opt 
int otlv4_trace_global = 0;

STD_NAMESPACE_PREFIX ostream &get_otlv4_trace_stream(void)
{
	return STD_NAMESPACE_PREFIX cerr;
}

void otl_str_to_tm(const char *str, otl_datetime &tm)
{
	tm.fraction = 0;
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	tm.tz_hour = 0;
	tm.tz_minute = 0;
#endif
	int rv = sscanf(str,
	                "%04d-%02d-%02d %02d:%02d:%02d.%03lu"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	                " %hd:%hd"
#endif
	                "",
	                &tm.year,
	                &tm.month,
	                &tm.day,
	                &tm.hour,
	                &tm.minute,
	                &tm.second,
	                &tm.fraction
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	                ,
	                &tm.tz_hour,
	                &tm.tz_minute
#endif
	               );
	tm.frac_precision = min(3, otl_odbc_date_scale);
	tm.fraction = otl_to_fraction(tm.fraction, tm.frac_precision);

	if (rv == 6)
	{
		rv = sscanf(str,
		            "%04d-%02d-%02d %02d:%02d:%02d"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		            " %hd:%hd"
#endif
		            "",
		            &tm.year,
		            &tm.month,
		            &tm.day,
		            &tm.hour,
		            &tm.minute,
		            &tm.second
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		            ,
		            &tm.tz_hour,
		            &tm.tz_minute
#endif
		           );
		tm.fraction = 0;
		tm.frac_precision = 0;
	}
}


void otl_ts_to_str(const otl_datetime &tm, char str[100])
{
	if (otl_odbc_date_scale == 0)
	{
		sprintf(str,
		        "%04d-%02d-%02d %02d:%02d:%02d"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        " %+hd:%hd"
#endif
		        "",
		        tm.year,
		        tm.month,
		        tm.day,
		        tm.hour,
		        tm.minute,
		        tm.second
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        ,
		        tm.tz_hour,
		        tm.tz_minute
#endif
		       );
	}
	else
	{
		int prec = min(3, otl_odbc_date_scale);

		sprintf(str,
		        "%04d-%02d-%02d %02d:%02d:%02d.%0*u"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        " %+hd:%hd"
#endif
		        "",
		        tm.year,
		        tm.month,
		        tm.day,
		        tm.hour,
		        tm.minute,
		        tm.second,
		        prec,
		        otl_from_fraction(tm.fraction, prec)
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        ,
		        tm.tz_hour,
		        tm.tz_minute
#endif
		       );
	}
}



STD_NAMESPACE_PREFIX ostream& otl_timestamp_to_ostream(STD_NAMESPACE_PREFIX ostream& dst, const otl_datetime &src)
{
	/* [i_a] ISO date/timestamp format so Old World blokes like me can grok it too ;-) */
   dst<<STD_NAMESPACE_PREFIX setfill('0')
	<<STD_NAMESPACE_PREFIX setw(4)<<src.year
	<<"/"<<STD_NAMESPACE_PREFIX setw(2)<<src.month
	<<"/"<<STD_NAMESPACE_PREFIX setw(2)<<src.day
	<<" "<<STD_NAMESPACE_PREFIX setw(2)<<src.hour
	<<":"<<STD_NAMESPACE_PREFIX setw(2)<<src.minute
	<<":"<<STD_NAMESPACE_PREFIX setw(2)<<src.second
	<<"."<<STD_NAMESPACE_PREFIX setw(src.frac_precision)<<otl_from_fraction(src.fraction, src.frac_precision)
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	<<" "
	<<(src.tz_hour>=0?"+":"")<<STD_NAMESPACE_PREFIX setw(2)<<src.tz_hour
	<<":"<<STD_NAMESPACE_PREFIX setw(2)<<src.tz_minute
#endif
	<<STD_NAMESPACE_PREFIX setfill(' ');

   return dst;
}




using namespace std;


void insert(const int buf_size)
// insert rows into table
{ 
 otl_stream o(buf_size,
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
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
  o<<i<<tmp;
 }
}

void update(const int af1, const int buf_size)
// insert rows into table
{ 
 otl_stream o(buf_size,
              "UPDATE test_tab "
              "   SET f2=:f2<char[31]> "
              " WHERE f1=:f1<int>", 
                 // UPDATE statement
              db // connect object
             );
 o.set_commit(0); // turn OFF the stream's "auto-commit"

 o<<"Name changed"<<af1;
 o<<otl_null()<<af1+1; // set f2 to NULL

}

void select(const int af1, const int buf_size)
{ 
 otl_stream i(buf_size,
              "select * from test_tab "
              "where f1>=:f11<int> "
              "  and f1<=:f12<int>*2",
                // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 char f2[31];

 i<<af1<<af1; // :f11 = af1, :f12 = af1
 while(!i.eof()){ // while not end-of-data
  i>>f1;
  cout<<"f1="<<f1<<", f2=";
  i>>f2;
  if(i.is_null())
   cout<<"NULL";
  else
   cout<<f2;
  cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

// ===================== Sybase via ODBC ======================================

  db.set_connection_mode(OTL_DEFAULT_ODBC_CONNECT);
  db.rlogon("scott/tigger@sybsql");

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

  db.commit();

  cout<<"============== Sybase via ODBC ====================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== TimesTen via ODBC ======================================

  db.set_connection_mode(OTL_TIMESTEN_ODBC_CONNECT);
  db.rlogon("scott/tiger@TT_tt70_32");

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

  db.commit();

  cout<<"============== TimesTen via TimesTen ODBC driver ==================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,1); // select records from the table, 
               // stream buffer size should be set to 1 
               // when connected to TimesTen via TimesTen ODBC driver
  cout<<"===================================================================="<<endl;

  db.logoff(); // disconnect from ODBC


// ===================== MS SQL 2008 via ODBC ==============================

  db.set_connection_mode(OTL_MSSQL_2008_ODBC_CONNECT);
  db.rlogon("scott/tiger@mssql2008");

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

  db.commit();

  cout<<"=========== MS SQL 2008 via ODBC ==================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== PostgreSQL via ODBC ==============================

  db.set_connection_mode(OTL_POSTGRESQL_ODBC_CONNECT);
  db.rlogon("scott/tiger@postgresql");

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

  db.commit();

  cout<<"=========== PostgreSQL via ODBC ==================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== EntipriseDB via ODBC ==============================

  db.set_connection_mode(OTL_ENTERPRISE_DB_ODBC_CONNECT);
  db.rlogon("scott/tiger@edbsql");

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

  db.commit();

  cout<<"=========== Enterprise DB via ODBC ================="<<endl;
  insert(10); // insert records into the table
  update(10,10); // update records in the table
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

// ===================== MySQL via ODBC ==============================

  db.set_connection_mode(OTL_MYODBC35_ODBC_CONNECT);
  db.rlogon("scott/tiger@mysql35");

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

  db.commit();

  cout<<"=========== MySQL via MyODBC 3.5 ==================="<<endl;
  insert(1); // insert records into the table, stream buffer size should be set to 1
  update(10,1); // update records in the table, stream buffer size should be set to 1
  select(8,10); // select records from the table
  cout<<"===================================================="<<endl;

  db.logoff(); // disconnect from ODBC

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}

