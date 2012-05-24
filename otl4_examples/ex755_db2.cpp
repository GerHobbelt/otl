/*
 * A copy of ex695, but now without setting up 'std' as the default namespace.
 *
 * (A similar example is provided for Oracle9 and ODBC.)
 *
 * OTLv4 before build 202 wouldn't survive.
 */
#include <iostream>
#include <stdio.h>

//#define OTL_STL
#define OTL_TRACE_LEVEL   0xFF
#define OTL_TRACE_STREAM  std::cerr
#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
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
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1;
 char f2[31];

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1;
  if(f1==4){
    i>>f2;
    std::cout<<"f1="<<f1<<", f2="<<f2<<std::endl;
  }else{
    i.skip_to_end_of_row(); // skipping to the end of the row
                            // when there is no need to read the remaining
                            // column values of the row
    std::cout<<"f1="<<f1<<", skipping the rest of the row"<<std::endl;
  }
 }

}

int main()
{
 otl_connect::otl_initialize();
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  std::cerr<<p.msg<<std::endl; // print out error message
  std::cerr<<p.stm_text<<std::endl; // print out SQL that caused the error
  std::cerr<<p.var_info<<std::endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;
}
