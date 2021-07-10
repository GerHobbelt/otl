#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#define OTL_ORA8I // Compile OTL 4/OCI8
#define OTL_STL // Turn on STL
#include <otlv4.h> // include the OTL 4 header file

#include <cstdio>
#include <string>
#include <iostream>

using namespace std;


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

 otl_int_vec t1; // PL/SQL table of int in the format of vector<int>
 otl_string_vec t2; // PL/SQL table of std::string in the vector format

 t1.set_len(3); // set t1's length to 3. It is important to call
                // this function BEFORE assigning values 
                // to the vector/PLSQL table

 t1.v[0]=1; // assign 1 to t1[0]
 t1.set_non_null(0); // specify that the value is not null

 t1.v[1]=2; // assign 2 to t1[1]
 t1.set_non_null(1); // specify that the value is not null

 t1.set_null(2); // set t1[2]=NULL
 s<<t1; // write t1 into the stream, since it is input

 s>>t1; // read t1 from the stream since it is input/output
 s>>t2; // read t2 from the stream since it is output.

 cout<<"T1_Len="<<t1.len()<<endl;
 for(int i=0;i<t1.len();++i)
  if(t1.is_null(i))
   cout<<"T1["<<i<<"]=NULL"<<endl;
  else
    cout<<"T1["<<i<<"]="<<t1.v[static_cast<size_t>(i)]<<endl;

 cout<<endl<<endl<<"T2_Len="<<t2.len()<<endl;
 for(int j=0;j<t2.len();++j)
  if(t2.is_null(j))
   cout<<"T2["<<j<<"]=NULL"<<endl;
  else
    cout<<"T2["<<j<<"]="<<t2.v[static_cast<size_t>(j)]<<endl;

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
    "  TYPE my_varchar_table IS TABLE OF VARCHAR2(31) INDEX BY BINARY_INTEGER; "
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
