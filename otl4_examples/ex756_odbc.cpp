#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <string>

#define OTL_ODBC_MSSQL_2008 // Compile OTL 4/MS SQL SNAC
#define OTL_STL // Turn on STL features
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif
#include <otlv4.h> // include the OTL 4 header file

using namespace std;

otl_connect db; // connect object

// my row class
class my_row_class{
public:
 int f1;
 string f2;

// default constructor
  my_row_class():f1(0),f2(){}

  // general purpose constructor
  my_row_class(const int af1, const string& af2):f1(af1),f2(af2){}

// destructor
 ~my_row_class(){}

// copy constructor
  my_row_class(const my_row_class& row):f1(row.f1),f2(row.f2){}

// copy assignment operator
 my_row_class& operator=(const my_row_class& row)
 {
   f1=row.f1;
   f2=row.f2;
   return *this;
 }

#if defined(OTL_CPP_11_ON) || defined(_MSC_VER) && (_MSC_VER >= 1600)
// when move constructors / assignment operators are supported

  my_row_class(my_row_class&& r):f1(r.f1),f2(move(r.f2)){}

  my_row_class& operator=(my_row_class&& r)
  {
    f1=r.f1;
    f2=move(r.f2);
    return *this;
  }

#endif

};

class my_set_less_than{
public:

  bool operator()(const my_row_class& row1, const my_row_class& row2) const
  {
    if(row1.f1<row2.f1)
      return true;
    else
      return false;
  }
};

// various STL containers
typedef vector<my_row_class> my_vector_type;
typedef map<int,string,less<int> > my_map_type;
typedef set<my_row_class,my_set_less_than> my_set_type;

// redefined operator>> for reading my_row_class& from otl_stream
otl_stream& operator>>(otl_stream& s, my_row_class& row)
{
 s>>row.f1>>row.f2;
 return s;
}

// redefined operator<< for writing const my_row_class& into
// otl_stream
otl_stream& operator<<(otl_stream& s, const my_row_class& row)
{
 s<<row.f1<<row.f2;
 return s;
}

// redefined operator>> for reading my_map_type::value_type& from
// otl_stream
otl_stream& operator>>(otl_stream& s, my_map_type::value_type& row)
{
  // std::map's key is const, which needs to be cast away
  s>>const_cast<int&>(row.first);
  s>>row.second;
 return s;
}

// redefined operator<< for writing const my_map_type::value_type&
// into otl_stream
otl_stream& operator<<(otl_stream& s, const my_map_type::value_type& row)
{
 s<<row.first<<row.second;
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

 my_vector_type vo; // vector of rows
 my_set_type so; // set of rows
 my_map_type mo; // map of pairs<int,string>

// populate the vector
 for(int i=1;i<=3;++i)
  vo.push_back(my_row_class(i,"NameXXX"));

// insert the vector into the table
 otl_write_to_stream(vo.begin(),vo.end(),o);


// populate the set
 for(int i=4;i<=6;++i)
  so.insert(my_row_class(i,"NameXXX"));

// insert the set into the table
 otl_write_to_stream(so.begin(),so.end(),o);

// populate the set
 for(int i=7;i<=10;++i)
  mo[i]="NameXXX";

// insert the map into the table
 otl_write_to_stream(mo.begin(),mo.end(),o);


}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f1<int> and f1<=:f11<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 my_vector_type v; // vector of rows
 my_set_type s; // set of rows
 my_map_type m;

// assigning :f1 = 4, :f11 = 4, and triggering an execution of the
// SELECT statement
 i<<4<<4;

// read all rows to be fetched into the vector
 otl_read_from_stream(i,back_inserter(v));    

 // print out the content of the vector
 for(my_vector_type::const_iterator x=v.begin(); x!=v.end(); ++x)
   cout<<"F1="<<(*x).f1<<", F2="<<(*x).f2<<endl;
 cout<<"==========================="<<endl;

// assigning :f1 = 4, :f11 = 4, and triggering an execution of the
// SELECT statement
 i<<4<<4;

// read all rows to be fetched into the set
 otl_read_from_stream(i,inserter(s,s.end()));    

 // print out the content of the set
 for(my_set_type::const_iterator x=s.begin(); x!=s.end(); ++x)
   cout<<"F1="<<(*x).f1<<", F2="<<(*x).f2<<endl;
 cout<<"==========================="<<endl;

// assigning :f1 = 4, :f11 = 4, and triggering an execution of the
// SELECT statement
 i<<4<<4;

// read all rows to be fetched into the set
 otl_read_from_stream(i,inserter(m,m.end()));    

 // print out the content of the set
 for(my_map_type::const_iterator x=m.begin(); x!=m.end(); ++x)
   cout<<"FIRST="<<(*x).first<<", SECOND="<<(*x).second<<endl;
 cout<<"==========================="<<endl;

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tiger@mssql2008"); // connect to the database

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

 db.logoff(); // disconnect from the database

 return 0;

}
