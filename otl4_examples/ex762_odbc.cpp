#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#define OTL_ODBC_ALTERNATE_RPC
#if !defined(_WIN32) && !defined(_WIN64)
#define OTL_ODBC
#else 
#define OTL_ODBC_POSTGRESQL // required with PG ODBC on Windows
#endif
#define OTL_STL
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object


const char* json_sample=
"{ \"PONumber\"             : 1600,"
"  \"Reference\"            : \"ABULL-20140421\","
"  \"Requestor\"            : \"Alexis Bull\","
"  \"User\"                 : \"ABULL\","
"  \"CostCenter\"           : \"A50\","
"  \"ShippingInstructions\" : { \"name\"   : \"Alexis Bull\","
"                             \"Address\": { \"street\"  : \"200 Sporting Green\","
"                                          \"city\"    : \"South San Francisco\","
"                                          \"state\"   : \"CA\","
"                                          \"zipCode\" : 99236,"
"                                          \"country\" : \"United States of America\" },"
"                             \"Phone\" : [ { \"type\" : \"Office\", \"number\" : \"909-555-7307\" },"
"                                         { \"type\" : \"Mobile\", \"number\" : \"415-555-1234\" } ] },"
"  \"Special Instructions\" : null,"
"  \"AllowPartialShipment\" : false,"
"  \"LineItems\"            : [ { \"ItemNumber\" : 1,"
"                               \"Part\"       : { \"Description\" : \"One Magic Christmas\","
"                                                \"UnitPrice\"   : 19.95,"
"                                                \"UPCCode\"     : 13131092899 },"
"                               \"Quantity\"   : 9.0 },"
"                             { \"ItemNumber\" : 2,"
"                               \"Part\"       : { \"Description\" : \"Lethal Weapon\","
"                                                \"UnitPrice\"   : 19.95,"
"                                                \"UPCCode\"     : 85391628927 },"
"                               \"Quantity\"   : 5.0 } ] }";

void insert()
// insert rows into table
{ 
 db.set_max_long_size(80000); // set maximum long string size for connect object
 otl_stream o(10, // buffer size
              "insert into test_tab values(:f1<int>,:f2<varchar_long>) ",
                 // SQL statement
              db // connect object
             );
 string json_str(json_sample);

 o<<1<<json_str;
 o<<2<<json_str;

}

void select()
{ 
 string f2;
 db.set_max_long_size(80000); // set maximum long string size for connect object

 otl_stream i(10, // buffer size
              "select f1, f2 :#2<varchar_long> "
              "from test_tab "
              "where f1>=:f11<int> "
              " and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;

 i<<1<<1; // assigning :f11 = 1, :f12 = 1
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 for(auto& it : i){ // while not end-of-data
 // while not end-of-data
  it>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
  cout<<"======================================================"<<endl;
 
}

}

int main()
{
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("scott/tiger@postgresql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 json)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}

