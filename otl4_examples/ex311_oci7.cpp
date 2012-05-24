#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA7 // Compile OTL 4.0/OCI7
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

 otl_int_tab<50000> t1; // PL/SQL table of int[50000]

void plsql(void)
{ 
 otl_stream s(1, // buffer size needs to be set to 1 in this case
                "begin "
                "  pkg_test.prc_test(:tsize<int,in>,:t1<int,out[50000]>); "
               "end;",
              db // connect object
             );

 s.set_commit(0); // Since there is no transactions, unset the stream auto-commit

 s<<50000; // write :tsize into the stream, since it is input
  s>>t1; // read :t1 from the stream since it is output

 cout<<"T1_Len="<<t1.len()<<endl;
 if(t1.is_null(0))
   cout<<"T1["<<0<<"]=NULL"<<endl;
 else
   cout<<"T1["<<0<<"]="<<t1.v[0]<<endl;
 if(t1.is_null(t1.len()-1))
   cout<<"T1["<<t1.len()-1<<"]=NULL"<<endl;
 else
   cout<<"T1["<<t1.len()-1<<"]="<<t1.v[t1.len()-1]<<endl;

}


int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("scott/tiger"); // connect to Oracle

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE pkg_test IS "
    "  TYPE my_numeric_table IS TABLE OF NUMBER INDEX BY BINARY_INTEGER; "
    "  PROCEDURE prc_test(tsize IN NUMBER, v1 OUT my_numeric_table); "
    " "
    "END; "
  );

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE BODY pkg_test IS "
    " "
    "  PROCEDURE prc_test(tsize IN NUMBER, v1 OUT my_numeric_table) "
    "  IS "
    "  BEGIN "
    "    FOR i IN 1..tsize LOOP "
    "      v1(i) := i; "
    "    END LOOP; "
    "  END; "
    "   "
    "END; "
  );


  plsql();

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
