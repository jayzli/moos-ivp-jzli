/*****************************************************************/
/*    NAME: John Li                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DensityCounter.h                                        */
/*    DATE: Mar 19th 2020                                        */
/*****************************************************************/
 
#ifndef DensityCounter_HEADER
#define DensityCounter_HEADER

#include <string>
#include <NodeRecord.h>
#include <ACTable.h>

using namespace std;

class DensityCounter
{
 public:
  DensityCounter();
  ~DensityCounter();

  // 
  void setRange(double v)       {m_range=v;}
  void setStep(double v)          {m_step=v;}
  
  int getDensity() const         {return(m_density);}
  string getName() const      {return(m_contact_name);}
  void calculateGoal();
  
 public:
  bool InRange(double m_range);
  double calRange();
  void AddContact (NodeRecord m_record);
  void IncrementStep (double m_step);
  string getReport(); //report state variables
  ACTable getDetailReport(); //report all contacts being tracked
  
  void setX(double v)                        {m_own_x=v;}
  void setY(double v)                        {m_own_y=v;}
  void setHeading(double v)          {m_own_heading=v;}
  void setSpeed(double v)             {m_own_speed=v;}
  void setOwnship(double x, double y, double hdg, double spd);
  
  void setGoalX(double v)                        {m_goal_x=v;}
  void setGoalY(double v)                       {m_goal_y=v;}
  void setGoal(double goal_x, double goal_y);

  int returnCount(double range, double step);
  
 protected: // State variables
  double  m_own_x ;
  double  m_own_y;
  double  m_own_heading;
  double  m_own_speed;
  
  double  m_goal_x ;
  double  m_goal_y;

  map<string, double> m_map_contact_x ;
  map<string, double> m_map_contact_y;
  map<string, double> m_map_contact_heading;
  map<string, double> m_map_contact_speed;
  map<string, double> m_map_contact_range;
  map<string, bool> m_map_in_range;
  string m_contact_name;

  double m_min_range;

  NodeRecord m_record;
  
  //Configuration Variables;
  double m_range;  // Range is the distance limit for density calculation 
  double m_step;  // Step is the temporal increment in seconds 
  double m_goal;  // Goal is the distance to destination
  
  int m_density; 
};

#endif 

