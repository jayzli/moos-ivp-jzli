/************************************************************/
/*    NAME: John Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: TrafficDensity.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TrafficDensity_HEADER
#define TrafficDensity_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <string>
#include <vector>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "DensityCounter.h"
//#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

using namespace std;
class TrafficDensity : public AppCastingMOOSApp
{
 public:
   TrafficDensity();
   ~TrafficDensity();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

public:
  bool buildReport();
  
 protected:
   void RegisterVariables();
   void handleMailNodeReport(std::string);
  void ChangeHeading(double m_heading_range);//not exactly accurate
  void ChangeSpeed(double m_speed_range);//not exactly accurate
  
  protected:
  //Configuration Variables, those set in my_vehicle.moos  file
  double m_range;  // Range is the distance limit for density calculation 
  double m_step;  // Step is the temporal increment in seconds 
  double m_heading_range;// Range of angles to consider
  double m_speed_range;// Range of speeds to consider
  
  //Goal is not a configuration variable for this app. 
 
  // State Variables
  double m_nav_x;
  double m_nav_y;
  double m_nav_hdg;
  double m_nav_spd;
  double m_goal;  // Goal is the distance to destination

  DensityCounter m_density_counter;

  int m_contact_count;
  
  string m_report;

 private: // Configuration variables

private: // State variables


  
};

#endif 
