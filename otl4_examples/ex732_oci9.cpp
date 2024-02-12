#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_UNICODE // Enable Unicode OTL for OCI9i

#define OTL_UNICODE_CHAR_TYPE wchar_t

#include <otlv4.h> // include the OTL 4.0 header file

const OTL_UNICODE_CHAR_TYPE* BigNumber=L"12345678901234567890123456789012345678";

class MyBigNumber{
public:

  MyBigNumber(): val_()
  {
    val_[0]=0;
  }

  MyBigNumber(const OTL_UNICODE_CHAR_TYPE* str_number): val_()
  {
    // convert str_number to internal format
    toMyBigNumber(str_number);
  }

  void toMyBigNumber(const OTL_UNICODE_CHAR_TYPE* str_number)
  {
    // convert str_number to internal format
    OTL_UNICODE_CHAR_TYPE* c2=&val_[0];   
    const OTL_UNICODE_CHAR_TYPE* c1=str_number;
    while(*c1)
     *c2++=*c1++;
    *c2=0;
  }

  void toString(OTL_UNICODE_CHAR_TYPE* str, int str_size) const
  {
    // convert from internal format to string format
    const OTL_UNICODE_CHAR_TYPE* c1=&val_[0];   
    OTL_UNICODE_CHAR_TYPE* c2=str;
    int str_len=0;
    while(*c1 && str_len<str_size-1){
     *c2++=*c1++;
     ++str_len;
    } 
    *c2=0;
  }

  ~MyBigNumber(){}

private:

  // internal stuff
  OTL_UNICODE_CHAR_TYPE val_[60];
};

otl_stream& operator>>(otl_stream& s, MyBigNumber& bn)
{
  OTL_UNICODE_CHAR_TYPE temp_val[60];
  s>>temp_val;
  for(OTL_UNICODE_CHAR_TYPE* c=temp_val;*c;++c)
    cout<<(char)*c;
  bn.toMyBigNumber(temp_val);
  return s;
}

otl_stream& operator<<(otl_stream& s, const MyBigNumber& bn)
{
  OTL_UNICODE_CHAR_TYPE str[60];
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

  db.rlogon("system/oracle@myora_tns"); // connect to database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 number)"
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
