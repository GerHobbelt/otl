#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_ODBC
//#define OTL_ODBC_UNIX // Compile OTL 4.0/ODBC
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
 int i,j;
 char tmp[32];

 otl_var_desc* vdesc;
 int vdesc_len;

 vdesc=o.describe_in_vars(vdesc_len);
 for(j=0;j<vdesc_len;++j){
  cout<<"IN VAR =========================>"<<j<<endl;
  cout<<"param_type="<<vdesc[j].param_type<<endl;
  cout<<"ftype="<<vdesc[j].ftype<<endl;
  cout<<"elem_size="<<vdesc[j].elem_size<<endl;
  cout<<"array_size="<<vdesc[j].array_size<<endl;
  cout<<"pos="<<vdesc[j].pos<<endl;
  cout<<"name_pos="<<vdesc[j].name_pos<<endl;
  cout<<"name="<<vdesc[j].name<<endl;
  cout<<"pl_tab_flag="<<vdesc[j].pl_tab_flag<<endl;
 }
 cout<<endl<<"Inserting rows into table..."<<endl;
 for(i=1;i<=5;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  cout<<"============= Inserting ========>"<<i<<endl;
  vdesc=o.describe_next_in_var();
  cout<<"ftype="<<vdesc->ftype<<", name="<<vdesc->name<<", f1="<<i<<endl;
  o<<i;
  vdesc=o.describe_next_in_var();
  cout<<"ftype="<<vdesc->ftype<<", name="<<vdesc->name<<", f2="<<tmp<<endl;
  o<<tmp;
 }

}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f<int> and f1<=:ff<int>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 int j;
 int f1;
 char f2[31];

 otl_var_desc* vdesc;
 int vdesc_len;

 vdesc=i.describe_in_vars(vdesc_len);
 for(j=0;j<vdesc_len;++j){
  cout<<"IN VAR =========================>"<<j<<endl;
  cout<<"param_type="<<vdesc[j].param_type<<endl;
  cout<<"ftype="<<vdesc[j].ftype<<endl;
  cout<<"elem_size="<<vdesc[j].elem_size<<endl;
  cout<<"array_size="<<vdesc[j].array_size<<endl;
  cout<<"pos="<<vdesc[j].pos<<endl;
  cout<<"name_pos="<<vdesc[j].name_pos<<endl;
  cout<<"name="<<vdesc[j].name<<endl;
  cout<<"pl_tab_flag="<<vdesc[j].pl_tab_flag<<endl;
 }

 vdesc=i.describe_out_vars(vdesc_len);
 for(j=0;j<vdesc_len;++j){
  cout<<"OUT VAR =========================>"<<j<<endl;
  cout<<"param_type="<<vdesc[j].param_type<<endl;
  cout<<"ftype="<<vdesc[j].ftype<<endl;
  cout<<"elem_size="<<vdesc[j].elem_size<<endl;
  cout<<"array_size="<<vdesc[j].array_size<<endl;
  cout<<"pos="<<vdesc[j].pos<<endl;
  cout<<"name_pos="<<vdesc[j].name_pos<<endl;
  cout<<"name="<<vdesc[j].name<<endl;
  cout<<"pl_tab_flag="<<vdesc[j].pl_tab_flag<<endl;
 }

 cout<<endl<<"Selecting rows from table..."<<endl;
 i<<2<<2; // assigning :f = 4, :ff = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  cout<<"======== Selecting a row ========>"<<endl;
  vdesc=i.describe_next_out_var();
  cout<<"ftype="<<vdesc->ftype<<", pos="<<vdesc->pos;
  i>>f1;
  cout<<", f1="<<f1<<endl;
  vdesc=i.describe_next_out_var();
  cout<<"ftype="<<vdesc->ftype<<", pos="<<vdesc->pos;
  i>>f2;
  cout<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the database API environment
 try{

  db.rlogon("scott/tigger@freetds_sybsql"); // connect to the database
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
  cerr<<p.sqlstate<<endl; // print out SQLSTATE message
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from teh database

 return 0;

}
