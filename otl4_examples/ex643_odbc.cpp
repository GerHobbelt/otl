#include <iostream>
#include <string.h>
using namespace std;

class user_defined_string{
public:

  user_defined_string(void)
  {
    buf[0]=0;
  }

  user_defined_string(const char* s)
  {
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(buf,sizeof(buf),s);
#else
    strcpy(buf,s);
#endif
#else
    strcpy(buf,s);
#endif
  }

  user_defined_string(const user_defined_string& s)
  {
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(buf,sizeof(buf),s.buf);
#else
    strcpy(buf,s.buf);
#endif
#else
    strcpy(buf,s.buf);
#endif
  }

  ~user_defined_string(){}

  user_defined_string& operator=(const char* s)
  {
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(buf,sizeof(buf),s);
#else
    strcpy(buf,s);
#endif
#else
    strcpy(buf,s);
#endif
    return *this;
  }

  void assign(int size,char filler)
  {
    for(int i=0;i<size;++i)
      buf[i]=filler;
    buf[size]=0;
  }

  void assign(char* temp_buf,int len)
  {
    for(int i=0;i<len;++i)
      buf[i]=temp_buf[i];
    buf[len]=0;
  }

  char& operator[](int ndx)
  {
    return buf[ndx];
  }

  int length(void) const 
  {
    return (int)strlen(buf);
  }

  user_defined_string& operator=(const user_defined_string& s)
  {
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(buf,sizeof(buf),s.buf);
#else
    strcpy(buf,s.buf);
#endif
#else
    strcpy(buf,s.buf);
#endif
    return *this;
  }

  const char* c_str(void) const
  {
    return buf;
  }


  user_defined_string& operator+=(const user_defined_string& s)
  {
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcat_s(buf,sizeof(buf),s.buf);
#else
    strcat(buf,s.buf);
#endif
#else
    strcat(buf,s.buf);
#endif
    return *this;
  }

protected:

   char buf[70000];

};

ostream& operator<<(ostream& s,const user_defined_string& str)
{
  s<<str.c_str();
  return s;
}

// Compile OTL 4.0/ODBC in Windows. Informix CLI uses the  
// regular ODBC header files in Windows
#define OTL_ODBC 

// Uncomment the #defines below in Linux / Unix
//#define OTL_ODBC_UNIX
//#define OTL_INFORMIX_CLI
#define OTL_VALUE_TEMPLATE_ON // Turn on otl_value<T>
#define OTL_USER_DEFINED_STRING_CLASS_ON
#define USER_DEFINED_STRING_CLASS user_defined_string
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab "
              "values(:f1<int>,:f2<char[31]>,:f3<timestamp>)", 
                 // SQL statement
              db // connect object
             );

 otl_value<user_defined_string> f2; // otl_value container with a string class
 otl_value<otl_datetime> f3; // container of otl_datetime


 for(int i=1;i<=100;++i){

  if(i%2==0)
   f2="NameXXX";
  else
   f2=otl_null(); // Assign otl_null() to f2

  if(i%3==0){
   // Assign a value to f3 via the .v field directly
   f3.v.year=2001;
   f3.v.month=1;
   f3.v.day=1;
   f3.v.hour=13;
   f3.v.minute=10;
   f3.v.second=5;
   f3.set_non_null(); // Set f3 as a "non-NULL"
  }else
   f3.set_null(); // Set f3 as null via .set_null() function

  o<<i<<f2<<f3;

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
 otl_value<user_defined_string> f2;
 otl_value<otl_datetime> f3;


 i<<8<<8; // assigning :f11 = :f12 = 8

   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3;
  cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<f3<<endl;
 }

 i<<4<<4; // assigning :f11 = :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2>>f3;
  cout<<"f1="<<f1<<", f2="<<f2<<", f3="<<f3<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("informix/tigger@informixsql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30), f3 datetime year to second)"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.sqlstate<<endl; // print out SQLSTATE
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
