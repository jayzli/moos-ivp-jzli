/************************************************************/
/*    NAME: John Li                                         */
/*    ORGN: MIT                                             */
/*    FILE: TrafficDensity.cpp                              */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "TrafficDensity.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TrafficDensity::TrafficDensity()
{
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

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
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
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
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
  //Register("CONTACT_RESOLVED", 0);                                               
  //Register("BCM_DISPLAY_RADII", 0);                                              
  //Register("BCM_ALERT_REQUEST", 0);                                              
  //Register("BCM_REPORT_REQUEST", 0);                                             
  Register("NAV_X", 0);                                                          
  Register("NAV_Y", 0);                                                          
  Register("NAV_SPEED", 0);                                                      
  Register("NAV_HEADING", 0);
}

