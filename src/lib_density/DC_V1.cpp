/*****************************************************************/
/*    NAME: John Li                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DensityCounter.cpp                                        */
/*    DATE: Mar 19th 2020                                        */
/*****************************************************************/

#include <iostream>
#include <cmath> 
#include "DensityCounter.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include <string>

using namespace std;

#define MPI 3.14159265359

//---------------------------------------------------------
// Constructor

DensityCounter::DensityCounter()
{
  m_own_x = 5;     // meters
  m_own_y = 5;
  m_own_heading=50;
  m_own_speed=10;
  m_goal_x=0;
  m_goal_y=0;

  m_contact_x=0 ;
  m_contact_y=0;
  m_contact_heading=0;
  m_contact_speed=0;

  double m_min_range = 10000; 

  m_step = .5;  //in seconds
  m_range = 50; //in meters
  
  m_density = 0;
}

//---------------------------------------------------------
// Destructor

DensityCounter::~DensityCounter()
{
  
}

//----------------------------------------------------------
// Check to see if a single contact is in range

bool DensityCounter::InRange(double m_range)
{
  bool in_range = false;
  calculateGoal();
  double step_limit = m_goal/m_own_speed/m_step;
  //Distance to goal divided by speed of own ship gives time to destination.
  //Time to distination divided by step size gives number of steps needed
  for (int i=1; i<step_limit; i++) {
    IncrementStep(m_step);
    double dis_x = m_contact_x - m_own_x;
    double dis_y = m_contact_y - m_own_y;
    double range = sqrt((dis_x*dis_x)+(dis_y*dis_y));
    if (range < m_range){
      in_range = true;
    }
  }

  return (in_range);
 }

//----------------------------------------------------------
//calculate range to contact 

double DensityCounter::calRange()
{
  calculateGoal();//distance to goal
  double step_limit = m_goal/m_own_speed/m_step;
  //Distance to goal divided by speed of own ship gives time to destination.
  //Time to distination divided by step size gives number of steps needed
  m_min_range=10000;
  for (int i=1; i<step_limit; i++) {
    IncrementStep(m_step);
    double dis_x = m_contact_x - m_own_x;
    double dis_y = m_contact_y - m_own_y;
    double range = sqrt((dis_x*dis_x)+(dis_y*dis_y));
    if (range < m_min_range){
      m_min_range = range;
    }
  }

  return (m_min_range);
 }

//----------------------------------------------------------
// Increment positions based on heading/speed
void DensityCounter::IncrementStep (double m_step)
 {
   m_contact_x = m_contact_x + m_step * m_contact_speed * cos(m_contact_heading*MPI/180);
   m_contact_y = m_contact_y + m_step * m_contact_speed * sin(m_contact_heading*MPI/180);

    m_own_x = m_own_x + m_step * m_own_speed * cos(m_own_heading*MPI/180);
    m_own_y =  m_own_y + m_step * m_own_speed * sin(m_own_heading*MPI/180);
}

//-------------------------------------------------------------
// Process noderecord class

void DensityCounter::ProcessRecord(NodeRecord m_record, bool contact)

{
  if (contact) {
    m_contact_x =  m_record.getX();
    m_contact_y =  m_record.getY();
    m_contact_heading =  m_record.getHeading();
    m_contact_speed =  m_record.getSpeed();
    m_contact_name = m_record.getName();
  }
  else {
    m_own_x =  m_record.getX();
    m_own_y =  m_record.getY();
    m_own_heading =  m_record.getHeading();
    m_own_speed =  m_record.getSpeed();
    //m_contact_name = m_record.getName();    m
  }
}

//----------------------------------------------------------
// Calculate distance to goal
void DensityCounter::calculateGoal ()
 {
    double dis_x = m_goal_x - m_own_x;
    double dis_y = m_goal_y - m_own_y;
    m_goal =  sqrt((dis_x*dis_x)+(dis_y*dis_y));
}

//-------------------------------------------------------------
string DensityCounter::getReport()
{
  string x = doubleToStringX(m_own_x);
  string y = doubleToStringX(m_own_y);
  string hdg = doubleToStringX(m_own_heading);
  string spd = doubleToStringX(m_own_speed);

  string c_x = doubleToStringX(m_contact_x);
  string c_y = doubleToStringX(m_contact_y);
  string c_hdg = doubleToStringX(m_contact_heading);
  string c_spd = doubleToStringX(m_contact_speed);

  string range = doubleToStringX(m_min_range);

  string report = "own ship x, y, hdg, spd: " + x +", " + y + ", " + hdg +"," + spd;
  report += ", " + m_contact_name + "'s  x, y, hdg, spd: " + c_x +", " +c_y + ", " +c_hdg +"," +c_spd;
  report += ", min range: " + range;

  return(report);
}
