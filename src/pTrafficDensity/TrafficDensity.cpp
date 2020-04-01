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

using namespace std;

//---------------------------------------------------------
// Constructor

TrafficDensity::TrafficDensity()
{
  // Configuration Variables
  m_range = 10; //default to 10 meters
  m_step = 10;//defaults to .5 seconds
  
  m_nav_x   = 0;                                                                 
  m_nav_y   = 0;                                                                 
  m_nav_hdg = 0;                                                                 
  m_nav_spd = 0;     

  m_report ="";
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
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TrafficDensity::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

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
  
  NodeRecord new_node_record = string2NodeRecord(report, true)
  


  m_report=report;
   
}

//---------------------------------------------------------
//Procedure: buildReport()
bool TrafficDensity::buildReport()
{
  m_msgs << "Newest report" << m_report<< endl;

  return(true);
}
