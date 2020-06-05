/************************************************************/
/*    NAME: John Li                                         */
/*    ORGN: MIT                                             */
/*    FILE: BHV_DensityCount.cpp                            */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_DensityCount.h"
#include "ZAIC_Vector.h"
#include <string>
#include <stdlib.h>
using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_DensityCount::BHV_DensityCount(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "density_count");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  m_density_str = "";
  // Add any variables this behavior needs to subscribe for
  addInfoVars("DENSITYCOUNT");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_DensityCount::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "foo") && isNumber(val)) {
    // Set local member variables here
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_DensityCount::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_DensityCount::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_DensityCount::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_DensityCount::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_DensityCount::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_DensityCount::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_DensityCount::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_DensityCount::onRunState()
{
  bool ok1;                                                                         
  m_density_str = getBufferDoubleVal("DENSITYCOUNT", ok1);                                                           
  if(!ok1) {   
    postWMessage("No Density Count provdiedownship");                      return(0);                  
  }                                                                                      
         

  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  ipf = buildFunctionWithZAICVector();

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

IvPFunction* BHV_DensityCount::buildFunctionWithZAICVector()
{
   ZAIC_Vector  zaic_vector(m_domain, "speed");
   vector<double> domain_vals;
   vector<double> range_vals;

   do{
     string str1=biteStringX(m_density_str, ':');
     string str2=biteStringX(m_density_str, ',');
     double val1=atof(str1.c_str());
     double val2=atof(str2.c_str());
     domain_vals.push_back(val1);
     range_vals.push_back(val2);
   } while (!m_density_str.empty());
   
   zaic_vector.setDomainVals(domain_vals);
   zaic_vector.setRangeVals(range_vals);
 
   IvPFunction *ivp_function = 0;
   //if(zaic_vector.stateOK())
   ivp_function = zaic_vector.extractIvPFunction();
     // else
     // cout << zaic_vector.getWarnings();
   return(ivp_function);
}
