#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif

#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_UNICODE // Enable Unicode OTL for OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void plsql(void)
{ 
 otl_stream s(1, // buffer size needs to be set to 1 in this case
                "begin "
                "  pkg_test.prc_test(:t1<int,inout[100]>,:t2<char[32],out[200]>); "
               "end;",
              db // connect object
             );

 s.set_commit(0); // Since there is no transactions, unset the stream auto-commit

 otl_int_tab<100> t1; // PL/SQL table of int[100]
 otl_cstr_tab<200,130> t2; 
  // PL/SQL table of char[200][64]
  // 130 = 31 * (2 bytes per Unicode character + 2 bytes for 
  // a possible surrogate character) + 2-byte null terminator

 t1.v[0]=1; // assign 1 to t1[0]
 t1.set_non_null(0); // specify that the value is not null

 t1.v[1]=2; // assign 2 to t1[1]
 t1.set_non_null(1); // specify that the value is not null

 t1.set_null(2); // set t1[2]=NULL
 t1.set_len(3); // set t1's length to 3

 s<<t1; // write t1 into the stream, since it is input

 s>>t1; // read t1 from the stream since it is input/output
 s>>t2; // read t2 from the stream since it is output.

 cout<<"T1_Len="<<t1.len()<<endl;
 for(int i=0;i<t1.len();++i)
  if(t1.is_null(i))
   cout<<"T1["<<i<<"]=NULL"<<endl;
  else
   cout<<"T1["<<i<<"]="<<t1.v[i]<<endl;

 cout<<endl<<endl<<"T2_Len="<<t2.len()<<endl;
 for(int j=0;j<t2.len();++j)
  if(t2.is_null(j))
   cout<<"T2["<<j<<"]=NULL"<<endl;
  else {
   cout<<"T2["<<j<<"]=";
// Character / string PL/SQL table containers can hold Unicode strings, when
// #define OTL_UNICODE is enabled.
   void* temp=&t2.v[j];
   unsigned short* t2jUni=reinterpret_cast<unsigned short*>(temp);
   for(int n=0;t2jUni[n]!=0;++n)
     cout<<static_cast<char>(t2jUni[n]); // Unicode to ASCII
   cout<<endl;
  }

}


int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("system/oracle@myora_tns"); // connect to Oracle

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE pkg_test IS "
    "  TYPE my_numeric_table IS TABLE OF NUMBER INDEX BY BINARY_INTEGER; "
    "  TYPE my_varchar_table IS TABLE OF VARCHAR2(128) INDEX BY BINARY_INTEGER; "
    "    /* size is defined in bytes for Oracle 8i... */ "
    " "
    "  PROCEDURE prc_test(v1 IN OUT my_numeric_table, v2 OUT my_varchar_table); "
    " "
    "END; "
  );

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE BODY pkg_test IS "
    " "
    "  PROCEDURE prc_test(v1 IN OUT my_numeric_table, v2 OUT my_varchar_table) "
    "  IS "
    "  BEGIN "
    "    FOR i IN 1..v1.last LOOP "
    "      v1(i) := v1(i)+100; "
    "    END LOOP; "
    "    v1(v1.last+1) := 0; "
    "    v2(1) := 'Hello'; "
    "    v2(2) := 'World'; "
    "    v2(3) := '!!!'; "
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
