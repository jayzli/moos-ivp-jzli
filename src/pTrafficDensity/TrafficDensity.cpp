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
  m_range = 10; //default to 10 meters
  m_step = .5;//defaults to .5 seconds
  m_heading_range  = 5;
  m_speed_range = 150;//15 knots max

  m_nav_x   = 0;                                                                 
  m_nav_y   = 0;                                                                 
  m_nav_hdg = 0;                                                                 
  m_nav_spd = 0;     

  m_report ="no vehicle in range";

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

  //trial code only considers increasing speed and heading
  //for (int i=1; i< m_heading_range; i++){
  //  ChangeHeading(i);
  for (int j=1; j<m_speed_range; j++){
      ChangeSpeed(j);
      SetOwnShip();
      int counter=0;
      // check to see if this particular contact will be in range 
      if ( m_density_counter.InRange(m_range)){
      //  m_report = m_density_counter.getName();
        m_report = m_report + "is in range";
	counter++;
      }
      m_contact_count.push_back(counter);
      
  }

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
        m_range = atof(value.c_str());
      }
      else if(param == "range_limit") {
        m_step = atof(value.c_str());
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
  if (vname == m_ownship)
    return;
  
  m_density_counter.ProcessRecord (new_node_record);
}

//---------------------------------------------------------
//Procedure: handleMailNodeReport()
void TrafficDensity::ChangeHeading(double m_heading_range)
{
  m_nav_hdg = m_nav_hdg + m_heading_range;
}


//---------------------------------------------------------
//Procedure: handleMailNodeReport()
void TrafficDensity::ChangeSpeed(double m_speed_range)
{
  m_nav_spd = m_speed_range/10;
}

//---------------------------------------------------------
//Procedure: handleMailNodeReport()
void TrafficDensity::SetOwnShip()
{
  m_density_counter.setX(m_nav_x);
  m_density_counter.setY(m_nav_y);
  m_density_counter.setHeading(m_nav_hdg);
  m_density_counter.setSpeed(m_nav_spd);
  m_density_counter.setGoalX(100);
  m_density_counter.setGoalY(-75);
}

//---------------------------------------------------------
//Procedure: buildReport()
bool TrafficDensity::buildReport()
{
  m_msgs <<m_ownship<< "'s speed choices" << endl;
  m_msgs <<"------------------------------------------"<< endl;
  ACTable actab(2,5);
  actab.setColumnJustify(1, "right");
  actab << "Speed | Peak Density";
  actab.addHeaderLines();
  //very hacky, need to pick a strcture to hold results
  for (double i =0 ; i < m_speed_range; i++) {
    string speed_str = doubleToStringX(i/10);
    string contacts = intToString(m_contact_count[i]);
    actab<<speed_str<<contacts;
   }
  m_msgs << actab.getFormattedString();
  
  return(true);
}
