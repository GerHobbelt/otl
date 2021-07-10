#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
// The following #define is required with MyODBC 3.51.11 and higher
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table and read database generated / auto-incremented 
// key values by calling a stored procedure that has 
// an output parameter
{ 
  otl_stream o(1, // buffer size
               "{call prc_1(@f1_1,:f2_2<char[31],in>)}",
               db
              );
  o.set_commit(0); // set stream auto-commit to OFF
  char f2_in[32]={0};
  int f1_out=0;

  // stream to read the value of the output parameter
  otl_stream s(1, "select @f1_1  :#1<int>", db, otl_implicit_select);

  for(int i=1;i<=10;++i) {
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
    sprintf_s(f2_in,sizeof(f2_in),"Name%d",i);
#else
    sprintf(f2_in,"Name%d",i);
#endif
#else
    sprintf(f2_in,"Name%d",i);
#endif
    o<<f2_in; // write input variable :f2

    s.rewind();
    while(!s.eof()){ // while not end-of-data
      s>>f1_out;
      cout<<"f1="<<f1_out<<", f2="<<f2_in<<endl;
    }
  }
  db.commit(); // commit transaction
 }

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=mysql35"); // connect to ODBC
//  db.rlogon("scott/tiger@mysql35"); // connect to ODBC, alternative format
                                    // of connect string 

    db.direct_exec
      ("drop table test_tab",
       otl_exception::disabled // disable OTL exceptions
      ); // drop table
    
    db.direct_exec
      ("CREATE TABLE test_tab "
       "(f1 INTEGER NOT NULL AUTO_INCREMENT, "
       " f2 VARCHAR(30),PRIMARY KEY (f1))"
      );  // create table

    db.direct_exec
      ("drop procedure prc_1",
       otl_exception::disabled // disable OTL exceptions
      ); // drop table

    db<<
      "create procedure prc_1 "
      " (out f11 int, "
      " in f22 varchar(30)) "
      "begin "
      " insert into test_tab(f2) values(f22); "
      " select last_insert_id() into f11; "
      " end; ";
    
    insert(); // insert records into table

 }catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
