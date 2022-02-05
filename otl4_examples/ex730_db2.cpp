#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#include <otlv4.h> // include the OTL 4.0 header file

const char* BigNumber="1234567890123456789.123";

class MyBigNumber{
public:

  MyBigNumber(): val_() 
  {
    val_[0]=0;
  }

  MyBigNumber(const char* str_number): val_()
  {
    // convert str_number to internal format
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(val_,sizeof(val_),str_number);
#else
    strcpy(val_,str_number);
#endif
#else
    strcpy(val_,str_number);
#endif
  }

  void toMyBigNumber(const char* str_number)
  {
    // convert str_number to internal format
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(val_,sizeof(val_),str_number);
#else
    strcpy(val_,str_number);
#endif
#else
    strcpy(val_,str_number);
#endif
  }

  void toString(char* str, 
#if defined(_MSC_VER)  && (_MSC_VER >= 1400)
                int str_size
#else
                int
#endif
    ) const
  {
    // convert from internal format to string format
#if defined(_MSC_VER) 
#if (_MSC_VER >= 1400)
    strcpy_s(str,str_size,val_);
#else
    strcpy(str,val_);
#endif
#else
    strcpy(str,val_);
#endif
    
  }

  ~MyBigNumber(){}

private:

  // internal stuff
  char val_[60];
};

otl_stream& operator>>(otl_stream& s, MyBigNumber& bn)
{
  char temp_val[60];
  s>>temp_val;
  cout<<temp_val;
  bn.toMyBigNumber(temp_val);
  return s;
}

otl_stream& operator<<(otl_stream& s, const MyBigNumber& bn)
{
  char str[60];
  bn.toString(str,sizeof(str));
  s<<str;
  return s;
}

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(5, // buffer size
              "insert into test_tab values(:f1<char[60]>)", 
                 // SQL statement
              db // connect object
             );
 MyBigNumber bn(BigNumber);
 for(int i=1;i<=10;++i){
   o<<bn;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select f1 :#1<char[60]> "
              "from test_tab",
                 // SELECT statement
              db // connect object
             ); 
 MyBigNumber bn;
   // create select stream
 while(!i.eof()){ // while not end-of-data
   cout<<"F1=";
   i>>bn;
   cout<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB API environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 decimal(22,3))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from database

 return 0;

}
