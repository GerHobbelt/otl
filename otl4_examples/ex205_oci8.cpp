#include <iostream>
using namespace std;
#include <stdio.h>
#include <string.h>

const int BASE_EXCEPTION=1; // "Base exception" class Id.
const int DB_BASE_EXCEPTION=2; // "DB Base exception" class Id.

// Let's assume that the following class is the base of 
// the system exception class hierarchy
class my_base_exception{
public:

  my_base_exception()
  {
    large_string_copy[0]=0;
  }

  virtual ~my_base_exception(){}

  virtual int getType(void) const
  {
    return BASE_EXCEPTION;
  }

  virtual int getErrorCode(void) const
  {
    return 0;
  }

  virtual const unsigned char* getErrorMessage(void) const
  {
    return reinterpret_cast<const unsigned char*>("");
  }

  virtual const char* getErrorSQLStm(void) const
  {
    return reinterpret_cast<const char*>("");
  }

  virtual const char* getErrorVarInfo(void) const
  {
    return reinterpret_cast<const char*>("");
  }

  virtual const char* getErrorLargeString(void) const
  {
    return large_string_copy;
  }

protected:

  char large_string_copy[100];

};

// Uncomment the line below when OCI7 is used with OTL
// #define OTL_ORA7 // Compile OTL 4.0/OCI7 

#define OTL_ORA8I // Compile OTL 4.0/OCI

// This #define connects the otl_exception class to the exception
// class hierarchy. 
#define OTL_EXCEPTION_DERIVED_FROM my_base_exception

// This #define defines a set of new members in the otl_exception
// class
#define OTL_EXCEPTION_HAS_MEMBERS                               \
  virtual int getType(void) const                               \
  {                                                             \
    return DB_BASE_EXCEPTION;                                   \
  }                                                             \
                                                                \
  virtual int getErrorCode(void) const                          \
  {                                                             \
    return this->code;                                          \
  }                                                             \
                                                                \
  virtual const unsigned char* getErrorMessage(void) const      \
  {                                                             \
    return this->msg;                                           \
  }                                                             \
                                                                \
  virtual const char* getErrorSQLStm(void) const                \
  {                                                             \
    return this->stm_text;                                      \
  }                                                             \
                                                                \
  virtual const char* getErrorVarInfo(void) const               \
  {                                                             \
    return this->var_info;                                      \
  }


#define OTL_EXCEPTION_COPIES_INPUT_STRING_IN_CASE_OF_OVERFLOW(str,len)       \
{                                                                            \
  copy_str(this->large_string_copy,str,len,sizeof(this->large_string_copy)); \
}

void copy_str(char* dest,const void* src, int len, int buf_size)
{
  int temp_len=len;
  if(temp_len>buf_size-1)
    temp_len=buf_size-1;
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  strncpy_s(dest,buf_size,reinterpret_cast<const char*>(src),temp_len);
  dest[temp_len+1]=0;
#else
  strncpy(dest,reinterpret_cast<const char*>(src),temp_len);
  dest[temp_len+1]=0;
#endif
#else
  strncpy(dest,reinterpret_cast<const char*>(src),temp_len);
  dest[temp_len+1]=0;
#endif
 
}

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<int>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[100];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"12345678901234567890123456789012345_%d",i);
#else
  sprintf(tmp,"12345678901234567890123456789012345_%d",i);
#endif
#else
  sprintf(tmp,"12345678901234567890123456789012345_%d",i);
#endif
  o<<i<<tmp;
 }
}

int main()
{
 otl_connect::otl_initialize(); // initialize the database environment
 try{

  db.rlogon("scott/tiger"); // connect to the database

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

 }
 catch(my_base_exception& ex){ // intercept the base exception
   if(ex.getType()==DB_BASE_EXCEPTION){ 
     // "database exception" is recognized
     // here, the otl_exception can be analyzed, or simply printed out
     cerr<<"Database exception:"<<endl;
     cerr<<"Code: "<<ex.getErrorCode()<<endl; // print out error code
     cerr<<"Message: "
         <<reinterpret_cast<const char*>(ex.getErrorMessage())
         <<endl; // print out the error message
     cerr<<"SQL Stm: "
         <<ex.getErrorSQLStm()
         <<endl; // print out the SQL Statement
     cerr<<"Var Info: "
         <<ex.getErrorVarInfo()
         <<endl; // print out the bind variable information
     if(ex.getErrorCode()==32005){
       // OTL defined exception
       cerr<<"Large Input String: "
           <<ex.getErrorLargeString()
           <<endl;
     }
   }else{
     // otherwise, do something else
     cerr<<"Base exception was caught..."<<endl;
   }
 }

 db.logoff(); // disconnect from the database

 return 0;

}
