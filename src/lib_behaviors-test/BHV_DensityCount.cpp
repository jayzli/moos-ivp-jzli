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
#include "XYFormatUtilsPoly.h"
#include "XYPoint.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_DensityCount::BHV_DensityCount(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "density_count");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "speed");

  m_density_str = "";

  // Visual Hint Defaults
  m_hint_vertex_size   = 1;
  m_hint_edge_size     = 1;
  m_hint_vertex_color  = "dodger_blue";
  m_hint_edge_color    = "white";
  m_hint_nextpt_color  = "yellow";
  m_hint_nextpt_lcolor = "aqua";
  m_hint_nextpt_vertex_size = 5;
  m_hint_poly_label="Traffic Lane";

  
  m_waypoint_engine.setPerpetual(true);
  m_waypoint_engine.setRepeatsEndless(true);

  m_goal_set = false;
  m_polygon_set = false;
  m_past_tpoint = true;
  m_in_poly = false;
  m_past_apoint = false;

  m_t_dist = 0;
  m_t_period = 60;
  m_a_dist = 0;
  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, NAV_SPEED, DENSITYUTIL, VIEW_POINT");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_DensityCount::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if(param == "polygon") {
    m_new_poly = string2Poly(val);
    cout << "new_poly size: " << m_new_poly.size() << endl;

    if(!m_new_poly.is_convex())  // Should be convex - false otherwise
      return(false);
    
    XYSegList new_seglist = m_new_poly.exportSegList(m_osx, m_osy);
    m_waypoint_engine.setSegList(new_seglist);

    m_polygon_set = true;
    return(true);
  }

  else if(param == "transition_distance") {
    double dval = atof(val.c_str());                                 
    if((dval < 0) || (!isNumber(val)))                                  
      return(false);                                                  
    m_t_dist = dval;
    return(true);
  }

  else if(param == "transition_period") {
    double dval = atof(val.c_str());                                 
    if((dval < 0) || (!isNumber(val)))                                  
      return(false);                                                  
    m_t_period = dval;
    m_a_dist = m_t_period * 1.5;
    string str1 = doubleToString(m_a_dist);
    postMessage("ACTIVATION_POINT", "Activation is  "+str1+" from t point");
    return(true);
  }
  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(val, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) 
      handleVisualHint(svector[i]);
    return(true);
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
  bool ok1, ok2, ok3, ok4;
  // ownship position in meters from some 0,0 reference point.
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  // Must get ownship information from the InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");  
  if(!ok3)
    postEMessage("No ownship HEADING info in info_buffer.");
  if(!ok4)
    postEMessage("No ownship SPEED info in info_buffer.");
  if(!ok1 || !ok2 || !ok3 || !ok4)
    return(0);
  
  bool ok5;                                              
  m_density_str = getBufferStringVal("DENSITYUTIL", ok5);                    
  if(!ok1) {   
    postEMessage("No Density Count provided for ownship");
    return(0);                  
  }
  
  bool ok6;
  string goal_str = getBufferStringVal("VIEW_POINT", ok6);
  if(!ok6)  {
    postEMessage("No goal info in info_buffer.");
    return(0);
  }
  else 
    handleViewPoint(goal_str);

  postViewablePolygon();
  inPolygon();

  string str1 = boolToString(m_past_tpoint);
  string str2 = boolToString(m_in_poly);
  postMessage("TPOINT_INPOLY", str1+str2);
  //only produce ipf if vessel is not in polygon and not past transition point
  if (!m_past_tpoint && !m_in_poly)  {
    // find and display transition point
    findTransitionPoint();

    // if before activation point don't return ipf
    if (!m_past_apoint)
      return(0);
    
     // Part 1: Build the IvP function
    IvPFunction *ipf = 0;
    ipf = buildFunctionWithZAICVector();

    if(ipf)
      ipf->setPWT(m_priority_wt);
    else                                                                   
    postEMessage("Unable to generate density counter IvP function");           
    
    return(ipf);
  }
  
  else {
    return(0);
  }
}

//---------------------------------------------------------------
// Procedure: buildFunctionWithZAICVector()
//   Purpose: 
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

//-----------------------------------------------------------
// Procedure: postViewablePolygon()
//      Note: Even if the polygon is posted on each iteration, the
//            helm will filter out unnecessary duplicate posts.

void BHV_DensityCount::postViewablePolygon()
{
  XYSegList seglist = m_waypoint_engine.getSegList();
  seglist.set_color("vertex", m_hint_vertex_color);
  seglist.set_color("edge", m_hint_edge_color);
  seglist.set_edge_size(m_hint_edge_size);
  seglist.set_vertex_size(m_hint_vertex_size);
  // Handle the label setting
  string bhv_tag = tolower(getDescriptor());
  bhv_tag = m_us_name + "_" + bhv_tag;
  seglist.set_label(bhv_tag);
  if(m_hint_poly_label == "")
    seglist.set_label(bhv_tag);
  else
    seglist.set_label(m_hint_poly_label);

  string poly_spec = seglist.get_spec();
  postMessage("VIEW_POLYGON", poly_spec);
}

//-----------------------------------------------------------
// Procedure: handleVisualHint()

void BHV_DensityCount::handleVisualHint(string hint)
{
  string param = tolower(stripBlankEnds(biteString(hint, '=')));
  string value = stripBlankEnds(hint);
  
  if((param == "vertex_size") && isNumber(value))
    m_hint_vertex_size = atof(value.c_str());
  else if((param == "edge_size") && isNumber(value))
    m_hint_edge_size = atof(value.c_str());
  else if((param == "vertex_color") && isColor(value))
    m_hint_vertex_color = value;
  else if((param == "edge_color") && isColor(value))
    m_hint_edge_color = value;
  else if((param == "nextpt_color") && isColor(value))
    m_hint_nextpt_color = value;
  else if((param == "nextpt_lcolor") && isColor(value))
    m_hint_nextpt_lcolor = value;
  else if((param == "nextpt_vertex_size") && isNumber(value))
    m_hint_nextpt_vertex_size = atof(value.c_str());
  else if(param == "label")
    m_hint_poly_label = value;
}

//---------------------------------------------------------
//Procedure: handleViewPoint()
void BHV_DensityCount::handleViewPoint(string val)
{
  vector<string> str_vector = parseString(val, ',');
  string x_val = str_vector[0]; 
  string y_val = str_vector[1];
  string str1=biteStringX(x_val, '=');
  string str2=biteStringX(y_val, '=');
  double goal_x = atof(x_val.c_str());
  double goal_y = atof(y_val.c_str());
  //determine if new goal is the same as previous goal
  if (m_goal_x != goal_x || m_goal_y != goal_y)  {
    m_goal_x = goal_x;
    m_goal_y = goal_y;
    m_goal_set = true;
    m_past_tpoint = false;
    m_past_apoint = false;
    postMessage("NEW_GOAL", val);
  }
}


//---------------------------------------------------------
//Procedure: findTransitionPoint()
void BHV_DensityCount::findTransitionPoint()
{
  if(!m_goal_set)
    postEMessage("Goal not set, can't calculate activation point");
  
  if(!m_polygon_set)
    postEMessage("Polygon not set, can't calculate activation point");

  if(m_goal_set && m_polygon_set) {
    double rel_ang =  relAng(m_osx, m_osy, m_goal_x, m_goal_y);
    double dist = m_new_poly.dist_to_poly(m_osx, m_osy, rel_ang);
    postMessage("DIST_TO_POLY", doubleToStringX(dist));

    if(m_a_dist > dist)
      m_past_apoint = true;
    else{
      m_past_apoint = false;
      double dist_to_a = dist - m_a_dist;
      m_a_point = projectPoint(rel_ang, dist_to_a, m_osx, m_osy);
    }
    
    if(dist > m_t_dist)  {
      dist -= m_t_dist;
      XYPoint end_pt = projectPoint(rel_ang, dist, m_osx, m_osy);
      m_seglist.clear(); 
      m_seglist.set_active(true);
      m_seglist.add_vertex(m_osx, m_osy);
      if (!m_past_apoint) {
        m_seglist.add_vertex(m_a_point);
      }
      m_seglist.add_vertex(end_pt);
      m_seglist.set_label("vector to transition point");
      postMessage("VIEW_SEGLIST", m_seglist.get_spec());
    }
    
    else {
      m_past_tpoint = true; 
      postMessage("PAST_TPOINT","Vessel past transition point");
    }
  }
}

//---------------------------------------------------------
//Procedure: inPolygon()
void BHV_DensityCount::inPolygon()
{
  if(!m_goal_set)
    postEMessage("Goal not set, can't calculate activation point");
  if(!m_polygon_set)
    postEMessage("Polygon not set, can't calculate activation point");

  if(m_goal_set && m_polygon_set) {
    if(m_new_poly.contains(m_osx, m_osy)) {
       m_in_poly = true;
       postMessage("IN_PLOYGON", "Vessel in traffic lane");
    }
    else {
       m_in_poly = false;
       postMessage("OUT_PLOYGON", "Vessel not inside traffic lane");
    }
  }
}
