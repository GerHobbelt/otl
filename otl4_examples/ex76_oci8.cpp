#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <vector>
#include <iterator>
#include <string>

#define OTL_ORA8I // Compile OTL 4/OCI8
#define OTL_STL // Turn on STL features
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif
#include <otlv4.h> // include the OTL 4 header file

using namespace std;

otl_connect db; // connect object

// row container class
class row{
public:
 int f1;
 string f2;

// default constructor
  row():
    f1(0),
    f2()
  {
  }

// destructor
 ~row(){}

// copy constructor
  row(const row& r):
    f1(r.f1),
    f2(r.f2)
 {
 }
 
// assignment operator
 row& operator=(const row& r)
 {
  f1=r.f1;
  f2=r.f2;
  return *this;
 }

};

// redefined operator>> for reading row& from otl_stream
otl_stream& operator>>(otl_stream& s, row& r)
{
 s>>r.f1>>r.f2;
 return s;
}

// redefined operator<< for writing row& into otl_stream
otl_stream& operator<<(otl_stream& s, const row& r)
{
 s<<r.f1<<r.f2;
 return s;
}

// redefined operator<< writing row& into ostream
ostream& operator<<(ostream& s, const row& r)
{
 s<<"f1="<<r.f1<<", f2="<<r.f2;
 return s;
}

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );

 row r; // single row buffer
 vector<row> vo; // vector of rows

// populate the vector
 for(int i=1;i<=100;++i){
  r.f1=i;
  r.f2="NameXXX";
  vo.push_back(r);
 }

 cout<<"vo.size="<<static_cast<int>(vo.size())<<endl;

// insert vector into table
 copy(vo.begin(), 
      vo.end(), 
      otl_output_iterator<row>(o)
     );

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:f*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 vector<row> v; // vector of rows

// assigning :f = 8
 i<<8;

// SELECT automatically executes when all input variables are
// assigned. First portion of out rows is fetched to the buffer

// copy all rows to be fetched into the vector

 copy(otl_input_iterator<row,ptrdiff_t>(i), 
      otl_input_iterator<row,ptrdiff_t>(),
      back_inserter(v));    

 cout<<"Size="<<static_cast<int>(v.size())<<endl;

// send the vector to cout
 copy(v.begin(), v.end(), ostream_iterator<row>(cout, "\n"));

// clean up the vector
 v.erase(v.begin(),v.end());

 i<<4; // assigning :f = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of out rows is fetched to the buffer

// copy all rows to be fetched to the vector
 copy(otl_input_iterator<row,ptrdiff_t>(i), 
      otl_input_iterator<row,ptrdiff_t>(),
      back_inserter(v));    

 cout<<"Size="<<static_cast<int>(v.size())<<endl;

// send the vector to cout
 copy(v.begin(), v.end(), ostream_iterator<row>(cout, "\n"));

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
