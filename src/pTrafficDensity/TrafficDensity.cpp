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
#include <string>

using namespace std;

//---------------------------------------------------------
// Constructor

TrafficDensity::TrafficDensity()
{
  // Configuration Variables
  m_range = 100; //default to 100 meters
  m_step = 1;//defaults to 1 second
  m_max_speed = 10;//defaults to 10 knots

  m_nav_x   = 0;                                                                 
  m_nav_y   = 0;                                                                
  m_nav_hdg = 0;                                                                 
  m_nav_spd = 0;     

  m_report ="no vehicle in range";

  m_ownship="nelson";

  m_count = 0;

  m_best_speed =0;

  m_goal_x = 0;

  m_goal_y = 0;
  
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
    else if(key == "VIEW_POINT")
      handleMailViewPoint(sval);
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

  m_density_counter.setGoal(m_goal_x, m_goal_y);//set goal to next waypoint

  m_density_counter.calCount();

  m_report = m_density_counter.getReport();
  m_detailed_report = m_density_counter.getDetailedReport();
  m_contact_report = m_density_counter.getContacts();
  
  string densityutil = m_density_counter.getUtilities();

  Notify("DENSITYUTIL",densityutil);

  string densitycount = m_density_counter.getDensities();
  Notify("DENSITYCOUNT",densitycount);
  
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
      string line = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
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
  Register("VIEW_POINT", 0);   
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
//Procedure: handleMailViewPoint()
void TrafficDensity::handleMailViewPoint(string val)
{
  vector<string> str_vector = parseString(val, ',');
  string x_val = str_vector[0]; 
  string y_val = str_vector[1];
  string str1=biteStringX(x_val, '=');
  string str2=biteStringX(y_val, '=');
  m_goal_x = atof(x_val.c_str());
  m_goal_y = atof(y_val.c_str());
  string goal = "x= " + x_val + ", y=" + y_val;
  Notify("GOAL", goal);
}
     
//---------------------------------------------------------
//Procedure: buildReport()
bool TrafficDensity::buildReport()
{
  m_msgs <<"---------------------------------------------------------"<< endl;
  m_msgs <<"Range limit set to " << doubleToStringX(m_range) << " meters"<<endl;
  m_msgs <<"Step Size is " << doubleToStringX(m_step) << " seconds"<<endl;
  m_msgs <<"---------------------------------------------------------"<< endl;

  string x = doubleToStringX(m_nav_x);
  string y = doubleToStringX(m_nav_y);
  string hdg = doubleToStringX(m_nav_hdg);
  string spd = doubleToStringX(m_nav_spd);
  string report  = x + "," +  y+ " , " + hdg +"," + spd;
  m_msgs<<"From App, own ship x, y, hdg, spd: "<<report<<endl;
  string goal = "x = " + doubleToStringX(m_goal_x) + ", y = " + doubleToStringX(m_goal_y);
  m_msgs<<"Goal location is: "<<goal<<endl;
  
  m_msgs <<"---------------------------------------------------------"<< endl;
  m_msgs<<"From Class: "<<m_report<<endl;
  
  m_msgs <<"---------------------------------------------------------"<< endl;
  m_msgs<<  m_contact_report.getFormattedString();
  m_msgs <<endl;
  m_msgs <<"---------------------------------------------------------"<< endl;
 m_msgs <<m_ownship<< "'s speed choices" << endl;
 m_msgs <<"---------------------------------------------------------"<<endl;
  m_msgs << m_detailed_report.getFormattedString();
   
   return(true);
}
