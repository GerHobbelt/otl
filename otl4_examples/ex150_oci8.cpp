#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <stdio.h>

#define OTL_ORA8I // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8i
//#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
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

void select()
{ 
 otl_stream i(1, // buffer size
              "begin "
              " my_pkg.my_proc(:f1<int,in>,:f2<int,in>, "
              "         :str1<char[100],out[200]>, "
      // :str1 is an output PL/SQL table of strings parameter
              "         :cur1<refcur,out[50]>, "
              "         :cur2<refcur,out[50]>); "
     // :cur1, :cur2 are a bind variable names, refcur -- their types, 
     // out -- output parameter, 50 -- the buffer size when this
     // reference cursor will be attached to otl_refcur_stream
              "end;", // PL/SQL block that calls a stored procedure
              db // connect object
             ); 

  i.set_commit(0); // set stream "auto-commit" to OFF.

 otl_cstr_tab<200,101> str1; // PL/SQL table of char[200][101]
 float f1=0;
 char f2[31];
 otl_refcur_stream s1, s2; // reference cursor streams for reading rows.

 i<<8<<4; // assigning :f1 = 8, :f2 = 4
 i>>str1; // reading :str1 from the stream
 i>>s1; // initializing the refeence cursor stream with the output 
       // reference cursor :cur1
 i>>s2; // initializing the refeence cursor stream with the output 
       // reference cursor :cur2

 cout<<"STR1_Len="<<str1.len()<<endl;
 for(int j=0;j<str1.len();++j)
  if(str1.is_null(j))
   cout<<"STR1["<<j<<"]=NULL"<<endl;
  else
   cout<<"STR1["<<j<<"]="<<str1.v[j]<<endl;

 cout<<"=====> Reading :cur1..."<<endl;
 while(!s1.eof()){ // while not end-of-data
  s1>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 cout<<"=====> Reading :cur2..."<<endl;
 while(!s2.eof()){ // while not end-of-data
  s2>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 s1.close(); // closing the reference cursor
 s2.close(); // closing the reference cursor

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
    "create table test_tab(f1 number, f2 varchar2(30))"
    );  // create table

  // create a PL/SQL package
  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE my_pkg IS "
    " "
    "  TYPE my_cursor IS REF CURSOR; "
    "  TYPE my_tab IS TABLE OF VARCHAR2(100) "
    "                 INDEX BY BINARY_INTEGER; "
    " "
    "  PROCEDURE my_proc "
    "    (f1_in IN NUMBER, "
    "     f2_in IN NUMBER, "
    "     str1 OUT my_tab, "
    "     cur1 OUT my_cursor, "
    "     cur2 OUT my_cursor); "
    " "
    "END; "
   );

  otl_cursor::direct_exec
   (db,
    "CREATE OR REPLACE PACKAGE BODY my_pkg IS "
    " "
    "  PROCEDURE my_proc "
    "    (f1_in IN NUMBER, "
    "     f2_in IN NUMBER, "
    "     str1 OUT my_tab, "
    "     cur1 OUT my_cursor, "
    "     cur2 OUT my_cursor) "
    "  IS "
    "    lv_cur1 my_cursor; "
    "    lv_cur2 my_cursor; "
    "  BEGIN "
    "    FOR i IN 1..4 LOOP "
    "      str1(i) := 'Line'||i; "
    "    END LOOP; "
    "    str1(5) := NULL; "
    "    OPEN lv_cur1 FOR "
    "      SELECT * FROM test_tab "
    "      WHERE f1>=f1_in  "
    "        AND f1<=f1_in*2; "
    "    OPEN lv_cur2 FOR "
    "      SELECT * FROM test_tab "
    "      WHERE f1>=f2_in  "
    "        AND f1<=f2_in*2; "
    "    cur1 := lv_cur1; "
    "    cur2 := lv_cur2; "
    "  END; "
    "   "
    "END; "
   );

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
