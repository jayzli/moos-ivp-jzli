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

using namespace std;

class DensityCounter
{
 public:
  DensityCounter();
  ~DensityCounter();

  void setRange(double v)       {m_range=v;}
  void setStep(double v)          {m_step=v;}
  void setGoal(double v)          {m_goal=v;}
  int getDensity() const         {return(m_density);}
  
 protected:
  bool InRange(double m_range);
  void ProcessRecord (NodeRecord m_record);
  void IncrementStep (double m_step);
  
 protected: // State variables
  double  m_own_x ;
  double  m_own_y;
  double  m_own_heading;
  double  m_own_speed;

  double  m_contact_x ;
  double  m_contact_y;
  double  m_contact_heading;
  double    m_contact_speed;

  NodeRecord m_record;

  //Configuration Variables;
  double m_range;  // Range is the distance limit for density calculation 
  double m_step;  // Step is the temporal increment in seconds 
  double m_goal;  // Goal is the distance to destination
  
  int m_density; 
};

#endif 

