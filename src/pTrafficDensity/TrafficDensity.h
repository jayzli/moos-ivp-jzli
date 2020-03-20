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
#include "NodeRecord.h"

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

 protected:
   void RegisterVariables();

  protected: // State Variables (Node Reports)
  std::string  m_helm_mode;
  std::string  m_helm_mode_aux;
  std::string  m_helm_allstop_mode;
  std::string  m_alt_nav_prefix;
  std::string  m_alt_nav_name;
  std::string  m_helm_status_primary;
  std::string  m_helm_status_standby;
  double       m_helm_lastmsg;

  NodeRecord   m_record;
  //NodeRecord   m_record_gt;
  double       m_record_gt_updated;

  double       m_nav_xy_updated;
  double       m_nav_latlon_updated;
  double       m_nav_xy_updated_gt;
  double       m_nav_latlon_updated_gt;

 private: // Configuration variables

 private: // State variables


  
};

#endif 
