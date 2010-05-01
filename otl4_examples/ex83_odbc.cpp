#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <cstring>
#include <cstdlib>

#define OTL_ODBC_MYSQL // Compile OTL 4/MyODBC
// #define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
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
  row(const row& arow):
    f1(arow.f1),
    f2(arow.f2)
 {
 }
 
// assignment operator
 row& operator=(const row& arow)
 {
  this->f1=arow.f1;
  this->f2=arow.f2;
  return *this;
 }

};

// redefined operator>> for reading row& from otl_stream
otl_stream& operator>>(otl_stream& s, row& row)
{
 s>>row.f1>>row.f2;
 return s;
}

// redefined operator<< for writing row& into otl_stream
otl_stream& operator<<(otl_stream& s, const row& row)
{
 s<<row.f1<<row.f2;
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
 otl_stream o(1, // buffer size should be == 1 on INSERT
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

void select(const int af1)
{ 

  char stmbuf[1024];
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(stmbuf,sizeof(stmbuf),
            "select * from test_tab where f1>=%d and f1<=%d*2",
            af1,
            af1
           );
#else
  sprintf(stmbuf,
          "select * from test_tab where f1>=%d and f1<=%d*2",
          af1,
          af1
         );
#endif
#else
  sprintf(stmbuf,
          "select * from test_tab where f1>=%d and f1<=%d*2",
          af1,
          af1
         );
#endif

 otl_stream i(50, // buffer size may be > 1
              stmbuf, // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 vector<row> v; // vector of rows


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
 otl_connect::otl_initialize(); // initialize ODBC environment
 try{

  db.rlogon("UID=scott;PWD=tiger;DSN=mysql"); // connect to ODBC

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
  select(8); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from ODBC

 return 0;

}
