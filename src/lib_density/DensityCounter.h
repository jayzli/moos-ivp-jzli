/*****************************************************************/
/*    NAME: John Li                                              */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DensityCounter.h                                     */
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
  void setRange(double v)                       {m_range=v;}
  void setStep(double v)                         {m_step=v;}
  void setMaxSpeed(double v)                {m_max_speed=v;}
  int getDensity() const                {return(m_density);}
  
 public:
  //bool InRange(double m_range);
  //double calRange();
  void AddContact (NodeRecord m_record);
  void IncrementStep (double m_step);
  string getReport(); //report state variables
  ACTable getDetailedReport(); //report detail results
  ACTable getContacts();//report all contacts being tracked
  string  getDensities();//return all speed and contact density pairs
  
  void setX(double v)                           {m_own_x=v;}
  void setY(double v)                           {m_own_y=v;}
  void setHeading(double v)               {m_own_heading=v;}
  void setSpeed(double v)                   {m_own_speed=v;}
  void setOwnship(double x, double y, double hdg, double spd);
  
  void setGoalX(double v)                      {m_goal_x=v;}
  void setGoalY(double v)                      {m_goal_y=v;}
  void setGoal(double goal_x, double goal_y);

  double HeadingToAngle(double heading);
  
  void calCount();
  void calculateGoal();
  
 protected: // State variables
  double  m_own_x ;
  double  m_own_y;
  double  m_own_heading;
  double  m_own_speed;
  
  double  m_goal_x ;
  double  m_goal_y;
  double m_goal_range;  // Goal is the distance to destination  double m
  double m_goal_heading; //heading pointing from own to goal

  //Vessel Based Maps, i.e. first string is the vessel name
  map<string, double> m_map_contact_x ;
  map<string, double> m_map_contact_y;
  map<string, double> m_map_contact_heading;
  map<string, double> m_map_contact_speed;
  map<string, double> m_map_contact_range;
  map<string, bool> m_map_in_range;
  
  //Speed Based Maps, i.e. key is the simulated vessel speed
  map<double, int> m_map_density_count;
  map<double, string> m_map_closest_contact;
  map<double, double> m_map_min_range;
  map<double, int> m_map_density_score;
  double m_min_range;

  NodeRecord m_record;
  
  //Configuration Variables;
  double m_range;  // Range is the distance limit for density calculation 
  double m_step;  // Step is the temporal increment in seconds 
  double m_max_speed; //Max speed to be considered
  
  int m_density; 
};

#endif 

