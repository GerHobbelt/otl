#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC // Compile OTL 4.0/ODBC
#define OTL_STREAM_READ_ITERATOR_ON
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 

 otl_long_string f2(200); // define long string variable
 otl_stream o(1, // buffer size still has to be set to 1
              "insert into test_tab values  "
              "(:f1<int>,:f2<raw[200]>), "
              "(:f12<int>,:f22<raw[200]>), "
              "(:f13<int>,:f23<raw[200]>), "
              "(:f14<int>,:f24<raw[200]>), "
              "(:f15<int>,:f25<raw[200]>) ",
// INSERT statement. Multiple sets of values can be used
// to work around the lack of the bulk interface
              db // connect object
             );

 for(int i=1;i<=20;++i){
  for(int j=0;j<200;++j)
   f2[j]='*';
  f2[199]='?';
  f2.set_len(200);
  o<<i<<f2;
 }

}

void select()
{ 
 otl_long_string f2(200); // define long string variable
 otl_stream i(10, // buffer size
              "select f1, "
// MyODBC does not return SQL_VARBINARY, so the output column type
// has to be overriden explicitly
              "       f2 :#2<raw[7200]> "
              "from test_tab "
              "where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 otl_stream_read_iterator<otl_stream,otl_exception,otl_lob_stream> rs;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
 rs.attach(i); // attach the iterator "rs" to the stream "i".
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(rs.next_row()){ // while not end-of-data
  rs.get(1,f1);
  rs.get(2,f2);
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(rs.next_row()){ // while not end-of-data
  rs.get(1,f1);
  rs.get(2,f2);
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[f2.len()-1]<<", len="<<f2.len()<<endl;
 }

 rs.detach(); // detach the itertor from the stream

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@mysql35"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 binary(200)) type=innoDB"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

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
