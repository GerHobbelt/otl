#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#endif
#include <iostream>
using namespace std;
#include <string.h>

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

  const char* c_str(void) const
  {
    return buf;
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

  void append(char* temp_buf, int len)
  {
    size_t cur_len=length();
    for(int i=0;i<len;++i)
      buf[cur_len+static_cast<size_t>(i)]=temp_buf[i];
    buf[cur_len+static_cast<size_t>(len)]=0;
  }


  char& operator[](int ndx)
  {
    return buf[ndx];
  }

  size_t length(void) const 
  {
    return strlen(buf);
  }


protected:

   char buf[60000];

};


ostream& operator<<(ostream& s,const user_defined_string& str)
{
  s<<str.c_str();
  return s;
}

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#define OTL_USER_DEFINED_STRING_CLASS_ON
#define USER_DEFINED_STRING_CLASS user_defined_string
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{

 user_defined_string f2; 
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
              "insert into test_tab values(:f1<int>,:f2<varchar_long>)",
                 // SQL statement
              db // connect object
             );

 for(int i=1;i<=20;++i){
  f2.assign(50001,' '); 
  for(int j=0;j<50000;++j)
   f2[j]='*';
  f2[50000]='?';
  f2[50001]=0;
  o<<i<<f2;
 }

}

void select()
{ 
 user_defined_string f2;
 db.set_max_long_size(70000); // set maximum long string size for connect object

 otl_stream i(1, // buffer size needs to be set to 1 for operations with LOBs
              "select * from test_tab where f1>=:f11<int> and f1<=:f12<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1=0;

 i<<8<<8; // assigning :f11 = 8, :f12 = 8
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[static_cast<int>(f2.length())-1]<<", len="
      <<static_cast<int>(f2.length())<<endl;
 }

 i<<4<<4; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2[0]<<f2[static_cast<int>(f2.length())-1]<<", len="
      <<static_cast<int>(f2.length())<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize DB2-CLI environment
 try{

  db.rlogon("scott/tiger@db2sql"); // connect to DB2-CLI

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",

    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 clob(1M))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from DB2-CLI

 return 0;

}
