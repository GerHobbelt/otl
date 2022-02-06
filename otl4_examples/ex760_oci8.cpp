#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <string>
using namespace std;

#if !defined(OTL_ORA12C)
#define OTL_ORA12C // Compile OTL 4.0/OCI12
#endif
#define OTL_STL // Enable STL compatibily mode
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

 string f2; 
 otl_stream o(3, 
              "insert into test_tab values(:f1<int>,:f2<char[4001]>)",
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

 otl_stream i(3,
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int f1=0;

 i<<1; // assigning :f = 1
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
  cout<<"======================================================"<<endl;
 }

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
    "create table test_tab(f1 number, f2 clob, check(f2 is json))"
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
