#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif


#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <cstring>
#include <cstdlib>

#define OTL_DB2_CLI // Compile OTL 4/DB2-CLI
#define OTL_STL // Turn on STL features
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif

#if (defined(__clang__) && (__clang_major__*100+__clang_minor__ >= 900)) && \
     (defined(OTL_CPP_14_ON))
#include <experimental/string_view>
#define OTL_STD_STRING_VIEW_CLASS std::experimental::string_view
#elif (defined(__clang__) && (__clang_major__*100+__clang_minor__ >= 900)) && \
     defined(OTL_CPP_17_ON)
#include <string_view>
#define OTL_STD_STRING_VIEW_CLASS std::string_view
#elif (defined(__clang__) && (__clang_major__*100+__clang_minor__ < 900) || defined(__GNUC__)) && \
     (defined(OTL_CPP_14_ON) || defined(OTL_CPP_17_ON))
#include <experimental/string_view>
#define OTL_STD_STRING_VIEW_CLASS std::experimental::string_view
#elif defined(_MSC_VER) && (_MSC_VER>=1910) && defined(OTL_CPP_17_ON)
// VC++ 2017 or higher when /std=c++latest is used
#include <string_view>
#define OTL_STD_STRING_VIEW_CLASS std::string_view
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
#if defined(OTL_STD_STRING_VIEW_CLASS)
  OTL_STD_STRING_VIEW_CLASS f2_sv(r.f2.c_str(),r.f2.length());
  s<<r.f1<<f2_sv;
#else
  s<<r.f1<<r.f2;
#endif
 return s;
}

// redefined operator<< writing row& into ostream
ostream& operator<<(ostream& s, const row& row)
{
 s<<"f1="<<row.f1<<", f2="<<row.f2;
 return s;
}

void insert()
// insert rows into table
{ 
 otl_stream o(5, // stream buffer size is logical rows
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // INSERT statement
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
 otl_stream i(5, // stream buffer size in logical rows
              "select * from test_tab "
              "where f1 between :f11<int> and :f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 vector<row> v; // vector of rows

 i<<8<<8; // enter :f11=8 and :f12=8 into the stream

// copy all rows to be fetched into the vector

 copy(otl_input_iterator<row,ptrdiff_t>(i), 
      otl_input_iterator<row,ptrdiff_t>(),
      back_inserter(v));    

 cout<<"Size="<<static_cast<int>(v.size())<<endl;

// send the vector to cout
 copy(v.begin(), v.end(), ostream_iterator<row>(cout, "\n"));

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2 environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=db2sql"); // connect to DB2

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
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from DB2

 return 0;

}
