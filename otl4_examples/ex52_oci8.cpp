#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4/OCI8
#include <otlv4.h> // include the OTL 4 header file

otl_connect db; // connect object

void plsql(void)
{ 
 otl_stream s(1, // buffer size needs to be set to 1 in this case
                "begin "
                "  pkg_test.prc_test(:v<int,in>, :t2<timestamp,out[200]>); "
               "end;",
              db // connect object
             );

 s.set_commit(0); // Since there is no transactions, unset the stream auto-commit

 otl_datetime_tab<200> t2; // PL/SQL table of otl_datetime[200]

 s<<7; // assign :v = 7
 s>>t2; // read :t2 from the stream.

 cout<<endl<<endl<<"T2_Len="<<t2.len()<<endl;
 for(int j=0;j<t2.len();++j)
  if(t2.is_null(j))
   cout<<"T2["<<j<<"]=NULL"<<endl;
  else
   cout<<"T2["<<j<<"]="<<t2.v[j].month
       <<"/"<<t2.v[j].day
       <<"/"<<t2.v[j].year
       <<" "<<t2.v[j].hour
       <<":"<<t2.v[j].minute
       <<":"<<t2.v[j].second
       <<endl;

}


int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("scott/tiger"); // connect to Oracle

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE pkg_test IS "
    "  TYPE my_date_table IS TABLE OF DATE INDEX BY BINARY_INTEGER; "
    " "
    "  PROCEDURE prc_test(v IN NUMBER, v2 OUT my_date_table); "
    " "
    "END; "
  );

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE BODY pkg_test IS "
    " "
    "  PROCEDURE prc_test(v IN NUMBER, v2 OUT my_date_table) "
    "  IS "
    "  BEGIN "
    "    FOR i IN 1..v LOOP "
    "      v2(i) := SYSDATE-i; "
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
