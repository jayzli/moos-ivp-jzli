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
  m_range = 100; //default to 10 meters
  m_step = .5;//defaults to .5 seconds
  m_heading_range  = 5;
  m_speed_range = 150;//15 knots max

  m_nav_x   = 10;                                                                 
  m_nav_y   = 10;                                                                
  m_nav_hdg = 180;                                                                 
  m_nav_spd = 5;     

  m_report ="no vehicle in range";

  m_ownship="nelson";

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
      string speed_str = doubleToStringX(m_nav_spd);
      int counter=0;
      string vname="no vessel in range";
      double min_cpa=m_range;
      
      // check to see if this particular contact will be in range 
      map<string, DensityCounter>::iterator q;
      for (q=m_map_density_counter.begin(); q!=m_map_density_counter.end(); q++){
        string     contact_name = q->first;
        DensityCounter density_counter  = q->second;

	//SetOwnShip(contact_name);  wrong place to set ship name
        m_report=density_counter.getReport();
	
	double cpa = density_counter.calRange();

	if (cpa<m_range){
	  counter++;
	  }
	if (cpa<min_cpa){
	  vname=contact_name;
	  min_cpa=cpa;
	  }
	
	//m_map_density_reports[speed_str]=density_counter.getReport();

	
	//Notify("DENSITY_REPORT", m_report);
        }
	//record result for this particulat speed

       m_map_contact_count[speed_str]=counter;
       m_map_closest_contact[speed_str]=vname;
       m_map_closest_CPA[speed_str]=min_cpa;

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

  #if 0
  if (vname == m_ownship){
    DensityCounter new_density_counter;
    new_density_counter.ProcessRecord (new_node_record, false);
    new_density_counter.setGoalX(100);
    new_density_counter.setGoalY(-75);
    m_map_density_counter[vname]=new_density_counter;   
  }
  #endif

  if  (vname == m_ownship){
    return;
  }
  
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
}
      

//---------------------------------------------------------
//Procedure: ChangeHeading()
void TrafficDensity::ChangeHeading(double m_heading_range)
{
  m_nav_hdg = m_nav_hdg + m_heading_range;
}


//---------------------------------------------------------
//Procedure: ChangeSpeed()
void TrafficDensity::ChangeSpeed(double m_speed_range)
{
  m_nav_spd = m_speed_range/10;
}

//---------------------------------------------------------
//Procedure: setOwnShip()
void TrafficDensity::SetOwnShip(string vname)
{
  m_map_density_counter[vname].setX(m_nav_x);
  m_map_density_counter[vname].setY(m_nav_y);
  m_map_density_counter[vname].setHeading(m_nav_hdg);
  m_map_density_counter[vname].setSpeed(m_nav_spd);
  m_map_density_counter[vname].setGoalX(100);
  m_map_density_counter[vname].setGoalY(-75);
}

//---------------------------------------------------------
//Procedure: buildReport()
bool TrafficDensity::buildReport()
{
  m_msgs <<"------------------------------------------"<< endl;
  m_msgs <<m_ownship<< "'s speed choices" << endl;
  m_msgs <<"------------------------------------------"<< endl;

  string x = doubleToStringX(m_nav_x);
  string y = doubleToStringX(m_nav_y);
  string hdg = doubleToStringX(m_nav_hdg);
  string spd = doubleToStringX(m_nav_spd);
  string report  = x + "," +  y+ " , " + hdg +"," + spd;
  m_msgs<<"From App, own ship x, y, hdg, spd: "<<report<<endl;
  
  m_msgs<<"From Class:"<<m_report<<endl;

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
