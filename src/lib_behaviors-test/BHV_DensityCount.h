/************************************************************/
/*    NAME: John Li                                         */
/*    ORGN: MIT                                             */
/*    FILE: BHV_DensityCount.h                              */
/*    DATE:                                                 */
/************************************************************/

#ifndef DensityCount_HEADER
#define DensityCount_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPolygon.h"
#include "WaypointEngine.h"

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

  void         handleVisualHint(std::string);
  void         postViewablePolygon();
  void         handleNextPoint(string val);
  void         findTransitionPoint();
  void         inPolygon();

private:
 
   // Visual Hint Defaults
  double       m_hint_vertex_size;
  double       m_hint_edge_size;
  std::string  m_hint_vertex_color;
  std::string  m_hint_edge_color;
  std::string  m_hint_nextpt_color;
  std::string  m_hint_nextpt_lcolor;
  double       m_hint_nextpt_vertex_size;
  std::string  m_hint_poly_label;

protected: // Local Utility functions
  IvPFunction* buildFunctionWithZAICVector();
 
protected: // Configuration parameters
  WaypointEngine m_waypoint_engine;
  XYPolygon    m_new_poly; //polygong representing traffic lane
  double       m_t_dist; //transition distance, default is 0
  double       m_t_period;//tranistion period, default is 60 secs
  double       m_a_dist;//activation distance

protected: // State variables
  string       m_density_str;
  double       m_osx;              // Curr owhship x/lon (mtrs)
  double       m_osy;              // Curr ownship y/lat (mtrs)
  double       m_osh;              // Curr ownship Heading (degs)
  double       m_osv;              // Curr ownship Speed (m/s)
  double       m_goal_x;
  double       m_goal_y;

  XYPoint      m_t_point;
  XYPoint      m_a_point;
  XYSegList    m_seglist;
  double       m_t_speed;
  
  bool         m_goal_set;
  bool         m_polygon_set;
  bool         m_past_tpoint;
  bool         m_in_poly;
  bool         m_past_apoint;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_DensityCount(domain);}
}
#endif
