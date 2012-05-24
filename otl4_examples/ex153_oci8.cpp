#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA8 // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8i
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object


void print_proc_type(const int stm_type)
{
  switch(stm_type){
  case otl_inout_stream_type:
    cout<<"OTL IN/OUT stream";
    break;
  case otl_refcur_stream_type:
    cout<<"OTL reference cursor stream";
    break;
  case otl_constant_sql_type:
    cout<<"OTL constant statement without any parameters";
    break;
  case otl_mixed_refcur_stream_type:
    cout<<"OTL mixed reference cursor stream";
    break;
  }
}

void stored_proc()
{ 
 otl_stream s; // stream variable, external to the create_stored_proc_call()

 char sql_stm[4096];
 int stm_type; 
 char refcur_placeholder[128];

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_proc1", // stored procedure name
    "my_pkg" // PL/SQL package name
   );
  cout<<"SQL_STM1="<<sql_stm<<endl;
  cout<<"STM_TYPE1=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME1="<<refcur_placeholder<<endl;
  cout<<endl;

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_proc2", // stored procedure name
    "my_pkg" // PL/SQL package name
   );
  cout<<"SQL_STM2="<<sql_stm<<endl;
  cout<<"STM_TYPE2=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME2="<<refcur_placeholder<<endl;
  cout<<endl;

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_func1", // stored function name
    "my_pkg" // PL/SQL package name
   );
  cout<<"SQL_STM3="<<sql_stm<<endl;
  cout<<"STM_TYPE3=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME3="<<refcur_placeholder<<endl;
  cout<<endl;

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_proc3", // stored procedure name
    "my_pkg" // PL/SQL package name
   );
  cout<<"SQL_STM4="<<sql_stm<<endl;
  cout<<"STM_TYPE4=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME4="<<refcur_placeholder<<endl;
  cout<<endl;

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_func2", // stored function name
    "my_pkg" // PL/SQL package name
   );
  cout<<"SQL_STM5="<<sql_stm<<endl;
  cout<<"STM_TYPE5=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME5="<<refcur_placeholder<<endl;
  cout<<endl;

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_proc4", // stored function name
    "my_pkg", // PL/SQL package name
    0, // schema name
    false, // schema name not included
    2001, // varchar paratemer default size
    otl_var_double, // default type to map NUMERIC parameters to
    100 // refernce cursor buffer size
   );
  cout<<"SQL_STM6="<<sql_stm<<endl;
  cout<<"STM_TYPE6=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME6="<<refcur_placeholder<<endl;
  cout<<endl;

  otl_stream::create_stored_proc_call 
   (db, // connect object
    s, // an external stream variable is needed here
    sql_stm, // output buffer for generating a stored procedure call
    stm_type, // output paremeter, indicating what type of stored procedure
    refcur_placeholder, // output parameter, which gets populated
                        // in the case of a stored procedure that returns
                        // a reference cursor.
    "my_func3", // stored function name
    "my_pkg", // PL/SQL package name
    0, // schema name
    false, // schema name not included
    2001, // varchar paratemer default size
    otl_var_double, // default type to map NUMERIC parameters to
    100 // refernce cursor buffer size
   );
  cout<<"SQL_STM7="<<sql_stm<<endl;
  cout<<"STM_TYPE7=";
  print_proc_type(stm_type);
  cout<<endl;
  cout<<"REF.CUR.NAME7="<<refcur_placeholder<<endl;
  cout<<endl;

}

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("scott/tiger"); // connect to Oracle

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number, f2 varchar2(30))"
    );  // create table

  // create a PL/SQL package
  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE my_pkg IS "
    " "
    "  TYPE my_cursor IS REF CURSOR; "
    " "
    "  PROCEDURE my_proc1; "
    "  PROCEDURE my_proc2 "
    "   (f1 IN NUMBER, "
    "    f2 IN OUT VARCHAR2, "
    "    f3 OUT DATE); "
    " "
    "  FUNCTION my_func1 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2) "
    "  RETURN DATE; "
    " "
    "  PROCEDURE my_proc3 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2, "
    "    f3 OUT my_cursor); "
    " "
    "  PROCEDURE my_proc4 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2, "
    "    f3 OUT NUMBER, "
    "    f4 OUT my_cursor, "
    "    f5 OUT my_cursor); "
    " "
    "  FUNCTION my_func2 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2) "
    "  RETURN my_cursor; "
    " "
    "  FUNCTION my_func3 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2, "
    "    f3 OUT NUMBER) "
    "  RETURN my_cursor; "
    "END; "
   );

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE BODY my_pkg IS "
    " "
    "  PROCEDURE my_proc1 "
    "  IS "
    "  BEGIN "
    "    NULL; "
    "  END; "
    "   "
    "  PROCEDURE my_proc2 "
    "   (f1 IN NUMBER, "
    "    f2 IN OUT VARCHAR2, "
    "    f3 OUT DATE) "
    "  IS "
    "  BEGIN "
    "    NULL; "
    "  END; "
    " "   
    "  FUNCTION my_func1 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2) "
    "  RETURN DATE "
    "  IS "
    "  BEGIN "
    "    RETURN SYSDATE; "
    "  END; "
    " "   
    "  PROCEDURE my_proc3 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2, "
    "    f3 OUT my_cursor) "
    "  IS "
    "  BEGIN "
    "    NULL; "
    "  END; "
    " "   
    "  PROCEDURE my_proc4 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2, "
    "    f3 OUT NUMBER, "
    "    f4 OUT my_cursor, "
    "    f5 OUT my_cursor) "
    "  IS "
    "  BEGIN "
    "    NULL; "
    "  END; "
    " "   
    "  FUNCTION my_func2 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2) "
    "  RETURN my_cursor "
    "  IS "
    "   lv_cur my_cursor; "
    "  BEGIN "
    "    OPEN lv_cur FOR "
    "    SELECT * FROM test_tab; "
    "    RETURN lv_cur; "
    "  END; "
    " "   
    "  FUNCTION my_func3 "
    "   (f1 IN NUMBER, "
    "    f2 IN VARCHAR2, "
    "    f3 OUT NUMBER) "
    "  RETURN my_cursor "
    "  IS "
    "   lv_cur my_cursor; "
    "  BEGIN "
    "    OPEN lv_cur FOR "
    "    SELECT * FROM test_tab; "
    "    RETURN lv_cur; "
    "  END; "
    " "   
    "END; "
   );

  stored_proc(); // generate a stored procedure call

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
