/************************************************************/
/*    NAME: John Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_DensityCount.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef DensityCount_HEADER
#define DensityCount_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_DensityCount : public IvPBehavior {
public:
  BHV_DensityCount(IvPDomain);
  ~BHV_DensityCount() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();

protected: // Local Utility functions
  IvPFunction* buildFunctionWithZAICVector();

protected: // Configuration parameters

protected: // State variables
  string m_density_str;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_DensityCount(domain);}
}
#endif
