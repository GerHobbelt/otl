#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>
#include <string.h>

// function that recognizes BACKUP and DBCC commands, and returns "true".
inline bool sql_statement_with_diag_rec_output(const char* stm_text)
{
  if(strncmp(stm_text,"BACKUP",6)==0)
    return true;
  else if(strncmp(stm_text,"DBCC",4)==0)
    return true;
  else
    return false;
}

// #define OTL_ODBC_MSSQL_2005 // Compile OTL 4/ODBC, MS SQL 2005
#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/ODBC, MS SQL 2008
#define OTL_ODBC_SQL_STATEMENT_WITH_DIAG_REC_OUTPUT sql_statement_with_diag_rec_output

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

int main()
{
 otl_connect::otl_initialize(); // initialize the environment
 try{

  db.rlogon("scott/tiger@mssql2008",1); 
   // connect to the database in the "auto-commit" mode
   // in order to avoid generating transactions.

  otl_nocommit_stream stm; 
   // the OTL "nocommit" stream doesn't generate any COMMITs,
   // which is needed for the BACKUP command.

  short int rec_ndx=1; // diagnostic record index
  SQLCHAR sql_state[1000]; // "SQL state" buffer
  SQLCHAR msg[SQL_MAX_MESSAGE_LENGTH+1]; // diagnostic record / message buffer
  int native_error; // native error code
  SQLRETURN ret; // return value (for SQLMoreResults() calls)

  const char* cmd = "BACKUP DATABASE master TO DISK='d:\\master.bak'";

  stm.open(1,cmd,db);
    // the stream recognizes the command as an "SQL statement 
    // with diagnostic record output" with the help of 
    // the sql_statement_with_diag_rec_output() function.
    // the stream executes the statement right away with an
    // SQLExecDirect() function call, which, in the case
    // of a BACKUP statement returns right away.

  bool done;
  // get all diagnostic records from the BACKUP command
  do{
    done=stm.get_next_diag_rec
           (rec_ndx,
            sql_state,
            msg,
            sizeof(msg),
            native_error);
    cout<<msg<<endl;
    // BACKUP command generates multiple results,
    // so we need to check if there are more results
    ret=SQLMoreResults(stm.get_stm_handle());
    if(ret==SQL_SUCCESS_WITH_INFO){
       done=false;
       rec_ndx=1; // in order to start a new diagnostic record
                  // sequence, the record index needs to be set
                  // to 1.
    }
  }while(!done);
  stm.close();

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.sqlstate<<endl; // print out SQLSTATE 
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
