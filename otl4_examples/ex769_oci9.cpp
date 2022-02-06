#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _ALLOW_RTCc_IN_STL 
#define _HAS_STD_BYTE 0
#define _HAS_STREAM_INSERTION_OPERATORS_DELETED_IN_CXX20 1
#endif

#include <iostream>
using namespace std;

#define OTL_STL // enables OTL/STL integration
#define OTL_ORA_SDO_GEOMETRY // enables support for Oracle MDSYS.SDO_GEOMETRY
#define OTL_ORA_OCI_ENV_CREATE // enables OCI mode that support "OCI object mode"
#define OTL_ORA_OCI_ENV_CREATE_MODE (OCI_THREADED|OCI_OBJECT) // enables OCI threaded and object modes
#if !defined(OTL_ORA11G_R2)
#define OTL_ORA11G_R2 // OCI 11.2
#endif
#include <otlv4.h>

otl_connect db;

void insert()
{
  otl_stream o(5, // buffer size
               "insert into test_tab "
               "values(:f1/*int*/,:f2/*sdo_geometry*/)",
               // SQL statement
               db // connect object
              );
  // Point
  {
    oci_spatial_geometry geom;
    geom.isNull = false;
    geom.x = 10;
    geom.y = 10;
    geom.gtype = 2001;//gType of Point2D
    o<<1<<geom;
  }

  // Line
  {
    oci_spatial_geometry geom;
    geom.isNull = false;
    geom.gtype = 2002;//gType of Line2D
    geom.eleminfo.push_back(1);
    geom.eleminfo.push_back(2);
    geom.eleminfo.push_back(1);
// array of coords, every pair = (x,y). 
// for such example line is {(100,100),(50,80),(40,40)}
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(50.0);
    geom.ordinates.push_back(80.0);
    geom.ordinates.push_back(40.0);
    geom.ordinates.push_back(40.0);
    o<<2<<geom;
  }
  
// Polygon
  {
    oci_spatial_geometry geom;
    geom.isNull = false;
    geom.gtype = 2003; //gType of Polygon2D
    std::vector<int> v1;
    geom.eleminfo.push_back(1);
    geom.eleminfo.push_back(1003);
    geom.eleminfo.push_back(1);
// array of coords, every pair = (x,y). 
// for such example polygon is {(100,100),(100,0),(0,0),(0,100),(100,100)}
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(0.0);
    geom.ordinates.push_back(0.0);
    geom.ordinates.push_back(0.0);
    geom.ordinates.push_back(0.0);
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(100.0);
    geom.ordinates.push_back(100.0);
    o<<3<<geom;
  }
  
}

void select()
{
  otl_stream i(5, // buffer size
               "select id,geoloc from test_tab",
               // SQL statement
               db // connect object
              );
  while(!i.eof()){
    int id;
    oci_spatial_geometry geom;
    i>>id>>geom;
    switch(geom.gtype){
    case 2001://read Point2D
      {
        cout<<"Point2D: X="<<geom.x<<", Y="<<geom.y<<endl;
        break;
      }
    case 2002://read line2D
      {
        cout<<"Line2D: ";
        cout<<"eleminfo(";
        for(size_t i2=0; i2<geom.eleminfo.size();++i2){
          cout<<geom.eleminfo[i2];
          if(i2<geom.eleminfo.size()-1)
            cout<<", ";
          else
            cout<<"), ";
        }
        cout<<"ordinates(";
        for(size_t i2=0; i2<geom.ordinates.size();++i2){
          cout<<geom.ordinates[i2];
          if(i2<geom.ordinates.size()-1)
            cout<<", ";
          else
            cout<<") ";
        }
        cout<<endl;
        break;
      }
    case 2003://read Polygon2D
      {
        cout<<"Polygon2D: ";
        cout<<"eleminfo(";
        for(size_t i2=0; i2<geom.eleminfo.size();++i2){
          cout<<geom.eleminfo[i2];
          if(i2<geom.eleminfo.size()-1)
            cout<<", ";
          else
            cout<<"), ";
        }
        cout<<"ordinates(";
        for(size_t i2=0; i2<geom.ordinates.size();++i2){
          cout<<geom.ordinates[i2];
          if(i2<geom.ordinates.size()-1)
            cout<<", ";
          else
            cout<<") ";
        }
        cout<<endl;
        break;
      }
    }
  }
  
}

int main()
{
  try{
    db.rlogon("system/oracle@myora_tns");
    db.direct_exec("drop table test_tab",otl_exception::disabled);
    db.direct_exec
      ("create table test_tab "
       "(id integer not null,"
       " geoloc mdsys.sdo_geometry not null, "
       " primary key (id))");
    db.direct_exec("delete from user_sdo_geom_metadata where table_name = 'TEST_TAB'");
    db.direct_exec
      ("insert into user_sdo_geom_metadata (table_name , column_name , diminfo , srid) "
       "values('TEST_TAB', 'GEOLOC', mdsys.sdo_dim_array(mdsys.sdo_dim_element('X', -100000, 100000, 0.001), "
       "mdsys.sdo_dim_element('Y', -100000, 100000, 0.001)), "
       "(select srid from mdsys.cs_srs where cs_name = 'Non-Earth (meters)')) ");
    insert();
    select();
    db.logoff();
  }catch(otl_exception& ex){
    cerr<<"Error Code:"<<ex.code<<endl;
    cerr<<"Error Msg:"<<ex.msg<<endl;
    cerr<<"Var Info:"<<ex.var_info<<endl;
  }
  return 0;
}
