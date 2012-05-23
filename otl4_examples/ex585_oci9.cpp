// User "scott" should have a CHANGE NOTIFICATION privilege.
//
// The required "change notification" privilege
// can be set by "grant change notification to scott" by the DBA.
//
// Active subscriptions may be viewed by
// "select * from USER_CHANGE_NOTIFICATION_REGS".
//------------------------------------------------------------------
#include <iostream>
using namespace std;
#include <time.h> // clock()

#if !defined(OTL_ORA10G_R2)
#define OTL_ORA10G_R2 // Compile OTL 4.0/OCI10gR2
#endif
#define OTL_ORA_SUBSCRIBE // Enable the otl_subscriber interface

// The following two #define's are required for 
// the otl_subscriber interface to function
#define OTL_ORA_OCI_ENV_CREATE 
#define OTL_ORA_OCI_ENV_CREATE_MODE (OCI_OBJECT|OCI_EVENTS)

#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

//------------------------------------------------------------------
// Function to log a message 
void Log(const char *sect, const char *mess, int x=-1 )
{
  cout << clock()/CLOCKS_PER_SEC << "\t:" << sect << ":\t" << mess;
  if(x>=0) cout << x;
  cout << '.' << endl;
}
//==================================================================

// Class derived from the otl_subscriber interface. 
// Implements an example of the Change Notification mechanism.
class TSubscriber : public otl_subscriber {
public:

  TSubscriber(otl_connect* adb): otl_subscriber(adb),select_str_(){}

  ~TSubscriber()
  {
   // Close the select stream in case if it was open
    if(select_str_.good()) select_str_.close();
  }

  void subscribe(void)
  {
    otl_subscriber::subscribe
      ("test_subs", // This subscription name (optional)
       5005, // Application listening TCP port number to receive
             // subscription notifications (optional)
       20 // Subscription expiration time, in seconds (optional, default
          // value 1800 sec, 0 means "never expire")
      );

    associate_query( "select :arg<int> from test_tab" ); 
    // "Touching" the table, which notification we would like to
    // receive, in the context of this subscription
    
    // associate_table( "test_tab2" ); 
    // There could be multiple tables in interest

   // Open a stream to be used by "Row events" functions

    if(!select_str_.good()){
      select_str_.open
        (1,
         "select f1 from test_tab where ROWID=:RId<char[250]>",
         *db);
    }
    Log( "Subs", "Subscription set" );
  }

private:

// Select stream for selecting changed rows
  otl_stream select_str_;


// The functions below override the pure virtual (interface)
// functions of the otl_subscriber interface 
  virtual void OnException(const otl_exception& e) 
  { 
    Log( "Event Exception", reinterpret_cast<const char*>(e.msg) ); 
  }

  virtual void OnDeRegistration(void)
  {
    Log( "Event", "Subscription time expired, resetting subscription" );
    if( is_online()) subscribe(); 
  }

  //--- DB events:
  virtual void OnStartup(void) 
  { 
    Log( "Event", "Database startup" ); 
  }

  virtual void OnInstanceShutdown(void) 
  { 
    Log( "Event", "This database instance shutdown" ); 
  }

  virtual void OnAnyInstanceShutdown(void) 
  { 
      Log( "Event", "Some database instance shutdown" ); 
  }

  //--- Table events:
  virtual void OnTableInvalidate(text * /*table_name*/)
  { 
    Log( "Event", 
         "Too many changes in data, table completely invalidated, "
         "no inidividual data notification follows" 
       );
  }

  virtual void OnTableAlter(text * /*table_name*/,bool /*all_rows*/ = false) 
  { 
    Log( "Event", "Table altered" ); 
  }

  virtual void OnTableDrop(text * /*table_name*/,bool /*all_rows*/ = false) 
  { 
    Log( "Event", "Table dropped" ); 
  }

  virtual void OnTableChange(text * /*table_name*/,bool /*all_rows*/ = false)
  { 
      Log( "Event", "Table changed" ); 
  }

  // followed by individual OnRow*() events for each data row

  //--- Row events:
  virtual void OnRowInsert(text *table_name, text *row_id)
  {
    // if the changed table name is "TEST_TAB" then 
    // select the row by its ROWID and then log the change
    if(strstr(reinterpret_cast<char*>(table_name),"TEST_TAB")){
      // here, there should be a mutex lock in case 
      // if the TSubscriber class is used from multiple threads
      select_str_<<row_id;
      while(!select_str_.eof()){
        double x;
        select_str_>>x;
        Log( "Event", "Row inserted, f1=", static_cast<int>(x) );
      }    
    }
  }

  virtual void OnRowUpdate( text *table_name, text *row_id )
  {
    // if the changed table name is "TEST_TAB" then 
    // select the row by its ROWID and then log the change
    if(strstr( reinterpret_cast<char*>(table_name), "TEST_TAB" )){
      // here, there should be a mutex lock in case 
      // if the TSubscriber class is used from multiple threads
      select_str_<<row_id;
      while(!select_str_.eof()){
        double x;
        select_str_>>x;
        Log( "Event", "Row updated, f1=", static_cast<int>(x) );
      }    
    }
  }

  virtual void OnRowDelete( text *table_name, text * /*row_id*/ )
  {
    // if the changed table name is "TEST_TAB" 
    // then log the change
    if( strstr( reinterpret_cast<char*>(table_name), "TEST_TAB" ) )
      Log( "Event", "Row deleted" );
  }

};

int main()
{
 otl_connect::otl_initialize(); // initialize OCI environment
 try{

  db.rlogon("scott/tiger"); // connect to Oracle
  db.direct_exec
    ("drop table test_tab", 
     otl_exception::disabled 
    ); // disable OTL exceptions
  db.direct_exec("create table test_tab(f1 number)" );
  TSubscriber subs(&db);
  subs.subscribe();
  //------------------------
  char str_buf[128];

  Log( "Main", "Inserting row, f1=", 123);
  db<<"insert into test_tab values(123)";
  db.commit();
  cout<<"Press 'x' and then press ENTER to continue or wait a bit, "<<endl
      <<"there may be pending notifications." << endl; 
  cin>>str_buf;
  cout << endl;
   
  Log( "Main", "Updating row, f1=", 456 );
  db<<"update test_tab set f1=456 where f1=123";
  db.commit();
  cout<<"Press 'x' and then press ENTER to continue or wait a bit, "<<endl
      <<"there may be pending notifications." << endl;
  cin>>str_buf;
  cout << endl;
  
  Log( "Main", "Deleting row, f1=", 456 );
  db<<"delete from test_tab where f1=456";
  db.commit();
  cout<<"Press 'x' and then press ENTER to quit or wait a bit, "<<endl
      <<"there may be pending notifications." << endl;
  cin>>str_buf;
  cout << endl;
    
  //------------------------
  subs.unsubscribe();


 }catch(otl_exception& ex){ // intercept OTL exceptions
  cerr<<ex.code<<endl; // print out error code
  cerr<<ex.msg<<endl; // print out error message
  cerr<<ex.stm_text<<endl; // print out SQL that caused the error
  cerr<<ex.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from Oracle

 return 0;

}
