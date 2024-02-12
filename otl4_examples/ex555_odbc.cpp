#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
#include <vector>
#include <iterator>
#include <string>

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__CYGWIN__)
#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC
#else 
#define OTL_ODBC
#endif
#define OTL_STL // Turn on STL features
#if !defined(OTL_ANSI_CPP) 
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif

#include <otlv4.h> // include the OTL 4.0 header file

using namespace std;

otl_connect db; // connect object

// row container class
class row{
public:
 int f1;
 string f2;

// default constructor
  row(): f1(0), f2(){}

// destructor
 ~row(){}

// copy constructor
  row(const row& arow): f1(arow.f1), f2(arow.f2){}
 
// assignment operator
 row& operator=(const row& arow)
 {
  f1=arow.f1;
  f2=arow.f2;
  return *this;
 }

};

// redefined operator>> for reading row& from otl_stream
otl_stream& operator>>(otl_stream& s, row& arow)
{
 s>>arow.f1>>arow.f2;
 return s;
}

// redefined operator<< for writing row& into otl_stream
otl_stream& operator<<(otl_stream& s, const row& arow)
{
 s<<arow.f1<<arow.f2;
 return s;
}

// redefined operator<< writing row& into ostream
ostream& operator<<(ostream& s, const row& arow)
{
 s<<"f1="<<arow.f1<<", f2="<<arow.f2;
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
              "select * from test_tab where f1>=:f1<int> and f1<=:f11<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 vector<row> v; // vector of rows

// assigning :f1 = 8, :f11 = 8
 i<<8<<8;

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

 i<<4<<4; // assigning :f1 = 4, :f11 = 4
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
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("sa/tigger@freetds_sybsql"); // connect to ODBC
  db.auto_commit_off();

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
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
