/************************************************************/
/*    NAME: John Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: TrafficDensity.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef TrafficDensity_HEADER
#define TrafficDensity_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class TrafficDensity : public CMOOSApp
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

 private: // Configuration variables

 private: // State variables
};

#endif 
