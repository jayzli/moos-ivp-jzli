 /************************************************************/
/*    NAME: John Li                                         */
/*    ORGN: MIT                                             */
/*    FILE: TrafficDensity.cpp                              */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TrafficDensity.h"
#include <list>
#include <stdlib.h>
#include "NodeRecordUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TrafficDensity::TrafficDensity()
{
  // Configuration Variables
  m_range = 50; //default to 100 meters
  m_step = .5;//defaults to .5 seconds
  m_max_speed = 15//defaults to 15 knots

  m_nav_x   = 0;                                                                 
  m_nav_y   = 0;                                                                
  m_nav_hdg = 0;                                                                 
  m_nav_spd = 0;     

  m_report ="no vehicle in range";

  m_ownship="nelson";

  m_count = 0;

  //m_contact_count[0] = 0;
  
}

//---------------------------------------------------------
// Destructor

TrafficDensity::~TrafficDensity()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TrafficDensity::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();

    if(key == "NAV_X")                                                           
      m_nav_x = dval;                                                            
    else if(key == "NAV_Y")                                                      
      m_nav_y = dval;                                                            
    else if(key == "NAV_HEADING")                                                
      m_nav_hdg = dval;                                                          
    else if(key == "NAV_SPEED")                                                  
      m_nav_spd = dval;                                                          
    else if(key == "NODE_REPORT")                                                
      handleMailNodeReport(sval);
    else                                                                         
      reportRunWarning("Unhandled Mail: " + key);    

   }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TrafficDensity::OnConnectToServer()
{
   RegisterVariables();
   
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TrafficDensity::Iterate()
{
  AppCastingMOOSApp::Iterate();

  m_density_counter.setOwnship(m_nav_x, m_nav_y, m_nav_hdg, m_nav_spd);
  m_density_counter.setRange(m_range);
  m_density_counter.setStep(m_step);
  m_density_counter.setMaxSpeed(m_max_speed);

  m_density_counter.setGoal(100, -75);//temporary place holder

  m_count=m_density_counter.returnCount(m_range, m_step);

  m_report = m_density_counter.getReport();
  m_detailed_report = m_density_counter.getDetailedReport();
  m_contact_report = m_density_counter.getContacts();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TrafficDensity::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  m_ownship = m_host_community;
  if(m_ownship == "") {
    cout << "Vehicle Name (MOOS community) not provided" << endl;
    return(false);
  }
  
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "step_size") {
        m_step = atof(value.c_str());
      }
      else if(param == "range_limit") {
        m_range = atof(value.c_str());
      }
      else if(param == "max_speed"){
	m_max_speed = atof(value.c_str());
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void TrafficDensity::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  
  Register("NODE_REPORT", 0);            
  Register("NAV_X", 0);                                                          
  Register("NAV_Y", 0);                                                          
  Register("NAV_SPEED", 0);                                                      
  Register("NAV_HEADING", 0);
}

//---------------------------------------------------------
//Procedure: handleMailNodeReport()
void TrafficDensity::handleMailNodeReport(string report)
{
  NodeRecord new_node_record = string2NodeRecord(report, true);
  string vname = new_node_record.getName();

  //if ownship, ignore
  if  (vname == m_ownship){
    return;
  }

  //else we process new node record with AddContact function from
  //DensityCounter class.
  m_density_counter.AddContact(new_node_record);
  return;

  //These are probably not needed
  #if 0
  //check to see if it is a new contact
  bool newly_known_vehicle = false;
  if(m_map_density_counter.count(vname) == 0)
    newly_known_vehicle = true; 
  //if new contact,  process using density counter
  if (newly_known_vehicle) {
    DensityCounter new_density_counter;
    new_density_counter.ProcessRecord (new_node_record, true);
    SetOwnShip(vname);
    m_map_density_counter[vname]=new_density_counter;
  }
  //if existing contact, update information
  if (! newly_known_vehicle)
    m_map_density_counter[vname].ProcessRecord(new_node_record, true);
  #endif
  
}
     

//---------------------------------------------------------
//Procedure: buildReport()
bool TrafficDensity::buildReport()
{
  m_msgs <<"------------------------------------------"<< endl;
  m_msgs <<"Range limit set to" << doubleToStringX(m_range) << "meters"<<endl;
  m_msgs <<"Step Size is" << doubleToStringX(m_step) << "seconds"<<endl;
  m_msgs <<m_ownship<< "'s speed choices" << endl;
  m_msgs <<"------------------------------------------"<< endl;

  string x = doubleToStringX(m_nav_x);
  string y = doubleToStringX(m_nav_y);
  string hdg = doubleToStringX(m_nav_hdg);
  string spd = doubleToStringX(m_nav_spd);
  string report  = x + "," +  y+ " , " + hdg +"," + spd;
  m_msgs<<"From App, own ship x, y, hdg, spd: "<<report<<endl;
  
  m_msgs <<"------------------------------------------"<< endl;
  m_msgs<<"From Class:"<<m_report<<endl;
  
  m_msgs <<"------------------------------------------"<< endl;
  m_msg<<  m_contact_report.getFormattedString();
  
  m_msgs <<"------------------------------------------"<< endl;
  m_msg<< m_detailed_report.getFormattedString();

  #if 0
    
  ACTable actab(4,5);
  actab.setColumnJustify(1, "right");
  actab << "Speed | Peak Density | Closest | min CPA";
  actab.addHeaderLines();

  map<string, int>::iterator p;
  for(p=m_map_contact_count.begin(); p!=m_map_contact_count.end(); p++) {
    string speed_str = p->first;
    double contact_count = p->second;
    string count_str = intToString(contact_count);
    string closest_contact = m_map_closest_contact[speed_str];
    string closest_cpa=  doubleToStringX(m_map_closest_CPA[speed_str]);
    actab<<speed_str<<count_str<<closest_contact<<closest_cpa;
   }
  m_msgs << actab.getFormattedString();

  #endif

  #if 0
   map<string, string>::iterator q;
   for(q=m_map_density_reports.begin(); q!=m_map_density_reports.end(); q++) {
      string speed_str = q->first;
      string report = q->second;
      m_msgs<<speed_str<<endl;
      m_msgs<<report<<endl;
   }

   #endif
   
   return(true);
}
