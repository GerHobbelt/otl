#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA8 // Compile OTL 4.0/OCI8
//#define OTL_ORA8I // Compile OTL 4.0/OCI8i
// #define OTL_ORA9I // Compile OTL 4.0/OCI9i
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
#define OTL_STREAM_READ_ITERATOR_ON
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
              " open :cur<refcur,out[50]> for "
     // :cur is a bind variable name, refcur -- its type, 
     // out -- output parameter, 50 -- the buffer size when this
     // reference cursor will be attached to otl_refcur_stream
              "  select * "
              "  from test_tab "
              "  where f1>=:f11<int,in> and f1<=:f12<int,in>*2; "
              "end;", // PL/SQL block returns a referenced cursor
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 i.set_commit(0); // turn off otl_stream's auto-commit
 
 int f1;
 char f2[31];
 otl_refcur_stream s;
 otl_stream_read_iterator<otl_refcur_stream,otl_exception,otl_lob_stream> rs;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer
 i>>s; // initialize the ref.cur. stream
 rs.attach(s); // attach the iterator "rs" to the stream "s".
 while(rs.next_row()){ // while not end-of-data
  rs.get(1,f1);
  rs.get(2,f2);
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
 i>>s; // initialize the ref.cur. stream
 rs.reattach(); // reattach the iterator "rs" to the stream "s". It's faster 
                // than detaching and attaching.
 while(rs.next_row()){ // while not end-of-data
  rs.get(1,f1);
  rs.get(2,f2);
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

 rs.detach(); // detach the itertor from the stream


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
