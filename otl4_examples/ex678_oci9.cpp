#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#define OTL_STL
#define OTL_ORA9I // Compile OTL 4.0/OCI9I
//#define OTL_ORA10G // Compile OTL 4.0/OCI10g
//#define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10gR2
//#define OTL_ORA11G // Compile OTL 4.0/OCI11G
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 db.set_max_long_size(20000); // set maximum long string size for connect object
 otl_stream o1(1, // buffer size needs to be set to 1
              "insert into temp_clob values(empty_clob()) "
	      "returning temp_clob into :temp_clob<clob>",
                 // SQL statement
              db // connect object
             );
 o1.set_commit(0); // set stream's auto-commit to OFF
 otl_stream o2(1, // buffer size needs to be set to 1
               "insert into test_tab "
	       "select :f1<int>, XMLType(temp_clob) from temp_clob",
               // SQL statement
               db // connect object
              );
 o2.set_commit(0); // set stream's auto-commit to OFF
 string f2_str;
 f2_str="<TAG>";
 string tmp_str;
 tmp_str.append(6000,'.');
 f2_str+=tmp_str;
 f2_str+="</TAG>";
 // insert the large XML into a temporary CLOB in a temporary table.
 o1<<f2_str;
 for(int i=1;i<=2;++i)
  o2<<i;
 db.commit(); // commit transaction

}

void select()
{ 
 string f2;
 db.set_max_long_size(20000); // set maximum long string size for connect object

 otl_stream i(10, // buffer size. To read XML as CLOBs, it can be set to a size greater than 1
              "select f1, xmltype.getclobval(f2) f2 "
              "from test_tab ",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

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
    "create table test_tab(f1 number, f2 xmltype)"
    );  // create table

// create global temporary table for temporary CLOB values
  db.direct_exec("drop table temp_clob",
                 otl_exception::disabled);
  db.direct_exec("create global temporary table temp_clob(temp_clob clob) "
                 "ON COMMIT delete rows");


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
