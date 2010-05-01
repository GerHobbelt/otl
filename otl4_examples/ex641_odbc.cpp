#include <iostream>
using namespace std;

#include <stdio.h>

// Compile OTL 4.0/ODBC in Windows. Informix CLI uses the  
// regular ODBC header files in Windows
#define OTL_ODBC 

// Uncomment the #defines below in Linux / Unix
//#define OTL_ODBC_UNIX
//#define OTL_INFORMIX_CLI
#define OTL_STL // turn on OTL in the STL compliance mode
#define OTL_STREAM_POOLING_ON 
 // turn on OTL stream pooling.
 // #define OTL_STREAM_POOLING_ON line 
 // can be commented out the number of iterations in
 // the select() loop can be increased, and the difference 
 // in performace with and without OTL_STREAM_POOLING_ON can
 // be benchmarked. The difference should grow with the overall
 // number of streams to be used in one program.

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
  sprintf(tmp,"Name%d",i);
  o<<i<<tmp;
 }
#ifdef OTL_STREAM_POOLING_ON
 o.close(false); // do not save the stream in the stream pool.
                 // in other words, destroy it on the spot, since
                 // the stream is not going to be reused later.
#else
 o.close();
#endif
}

void select()
{ // when this function is called in a loop,
  // on the second iteration of the loop the streams i1, i2 will
  // will get the instances of the OTL stream from the stream
  // pool, "fast reopen", so to speak.

 otl_stream i1(50, // buffer size
               "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 otl_stream i2(33, // buffer size
               "select f1,f2 from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream

// i1 and i2 are NOT similar, because their buffer sizes as well
// as SQL statements are not equal. It will generate two entry points in the
// OTL stream pool.
 
 int f1;
 char f2[31];

 i1<<2<<2; // assigning :f11 = 2, :f12 = 2
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i1.eof()){ // while not end-of-data
  i1>>f1>>f2;
  cout<<"I1==> f1="<<f1<<", f2="<<f2<<endl;
 }

 i2<<3<<3; // assigning :f11 = 2, :f12 = 2
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i2.eof()){ // while not end-of-data
  i2>>f1>>f2;
  cout<<"I2==> f1="<<f1<<", f2="<<f2<<endl;
 }

} // destructors of i1, i2 will call the close()
  // function for both of the streams and the OTL stream
  // instances will be placed in the stream pool.

int main()
{
 otl_connect::otl_initialize(); // initialize the environment
 try{

  db.rlogon("informix/tigger@informixsql"); // connect to the database
#ifdef OTL_STREAM_POOLING_ON
  db.set_stream_pool_size(2); 
   // set the maximum stream pool size and actually initializes 
   // the stream pool.
   // if this function is not called, the stream pool
   // gets initialized anyway, with the default size of 32 entries.
#endif

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
  for(int i=1;i<=10; ++i){
    cout<<"===================> Iteration: "<<i<<endl;
    select(); // select records from table
  }
 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
