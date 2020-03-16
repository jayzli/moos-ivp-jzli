/************************************************************/
/*    NAME: John Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
   m_first_reading = true;
   m_current_x=0.0;
   m_current_y=0.0;
   m_previous_x=0.0;
   m_previous_y=0.0;
   m_total_distance=0.0;
   
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
 
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();

    if (key == "NAV_X") {
      m_previous_x = m_current_x;
      m_current_x = dval;
    }
    else if (key == "NAV_Y") {
      m_previous_y = m_current_y;
      m_current_y = dval;
    };
	
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{
  //Ignore the initial reading, due to initialized values for m_current_x and m_current_y
  if(m_first_reading) {
    m_first_reading = false;
  }
  //Determine if the X or Y positions have changed since last iteration
  else {
  double distance_x = m_current_x - m_previous_x; // Change in x position
  double distance_y = m_current_y - m_previous_y; // Change in y position

  double distance = sqrt((distance_x * distance_x) + (distance_y * distance_y)); //Distance = Total change in distance
  
  m_total_distance = m_total_distance + distance; // Total distance traveled
    }
  
  Notify("ODOMETRY_DIST", m_total_distance);

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
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

void Odometry::RegisterVariables()
{
  // AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);

}

