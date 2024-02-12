#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif
#include <iostream>
#include <thread>
using namespace std;

#include <stdio.h>

#define OTL_DB2_CLI // Compile OTL 4.0/DB2-CLI
#define OTL_CONNECT_POOL_ON
#include <otlv4.h> // include the OTL 4.0 header file

typedef otl_connect_pool<otl_connect,otl_exception> ConnectPoolType;

void f1(ConnectPoolType* ptr)
{
  ConnectPoolType& pool=*ptr;
  ConnectPoolType::connect_ptr p1=pool.get();
  if(!p1){
    cerr<<"p1 is empty<<"<<endl;
   return;
  }
  try{
    cout<<"Using a connect from the pool for the first time..."<<endl;
    p1->direct_exec("drop table test_tab",0);
    p1->direct_exec("create table test_tab(f1 int)");
    otl_stream o(3,"INSERT INTO test_tab VALUES(:f1<int>)",*p1);
    o<<10;
    o.flush();
    o.close();
    otl_stream s(3,"SELECT f1 FROM test_tab",*p1);
    int f1=0;
    while(!s.eof()){
      s>>f1;
      cout<<"F1="<<f1<<endl;
    }
  }catch(otl_exception& p){ // intercept OTL exceptions
    cerr<<p.msg<<endl; // print out error message
    cerr<<p.stm_text<<endl; // print out SQL that caused the error
    cerr<<p.sqlstate<<endl; // print out SQLSTATE message
    cerr<<p.var_info<<endl; // print out the variable that caused the error
  }
  pool.put(std::move(p1));
}

void f2(ConnectPoolType* ptr)
{
  ConnectPoolType& pool=*ptr;
  ConnectPoolType::connect_ptr p2=pool.get();
  if(!p2){
    cerr<<"p2 is empty<<"<<endl;
   return;
 }
 try{
    cout<<"Using a connect from the pool for the second time..."<<endl;
    p2->direct_exec("drop table test_tab2",0);
    p2->direct_exec("create table test_tab2(f1 int)");
    otl_stream o(3,"INSERT INTO test_tab2 VALUES(:f1<int>)",*p2);
    o<<10;
    o.flush();
    o.close();
    otl_stream s(3,"SELECT f1 FROM test_tab2",*p2);
    int f1=0;
    while(!s.eof()){
      s>>f1;
      cout<<"F1="<<f1<<endl;
    }
  }catch(otl_exception& p){ // intercept OTL exceptions
    cerr<<p.msg<<endl; // print out error message
    cerr<<p.stm_text<<endl; // print out SQL that caused the error
    cerr<<p.sqlstate<<endl; // print out SQLSTATE message
    cerr<<p.var_info<<endl; // print out the variable that caused the error
  }
  pool.put(std::move(p2));
}

int main()
{
 otl_connect::otl_initialize(1); // initialize the database API for multi-threaded environment
 try{
   ConnectPoolType pool;
// open a pool of 8 connections with auto_commit set to false.
   pool.open("scott/tiger@db2sql",false,8);
// spawn two threads and call f1() and f2() concurrently.
   thread t1(f1,&pool);
   thread t2(f2,&pool);
// wait until both threads are finished.
   t1.join();
   t2.join();
// shrink the minimum pool size from 8 to 4
   pool.shrink_pool(4);
// close the pool.
   pool.close();
 }catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }


 return 0;

}
