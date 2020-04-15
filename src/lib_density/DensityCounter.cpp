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
  m_own_x = 0;     // meters
  m_own_y = 0;
  m_own_heading=0;
  m_own_speed=0;
  
  m_goal_x=0;
  m_goal_y=0;

  m_min_range = 10000; 

  m_step = 1;  //in seconds
  m_range = 50; //in meters
  m_max_speed = 15; //max speed in knots
  
  m_density = 0;
}

//---------------------------------------------------------
// Destructor

DensityCounter::~DensityCounter()
{
  
}

//----------------------------------------------------------
// Check to see if a single contact is in range

void DensityCounter::calCount()
{
  //m_range = range;
  //m_step = step;
  
  for (int j=1; j<=m_max_speed*10.01; j++){
      double sim_speed = j/10; //simulated speed go from .1 to max speed 
      string speed_str = doubleToStringX(sim_speed);
      int counter=0;
      double min_cpa=m_range;
      string closest = "no vessel in range";
      // calculate range and heading to goal
      calculateGoal();

      // loop through contacts will be in range
      map<string, double>::iterator q;
      for (q=m_map_contact_x.begin(); q!=m_map_contact_y.end(); q++){
        string     vname = q->first;
        double  contact_x = q->second;
        double  contact_y = m_map_contact_y[vname];
	double  contact_hdg = m_map_contact_heading[vname];
	double  contact_spd = m_map_contact_speed[vname];
						  
	double  contact_angle = HeadingToAngle(contact_hdg);

	double own_x = m_own_x;
	double own_y = m_own_y;
						  
	//Distance to goal divided by simulated speed of own ship gives time to destination.
	//Time to distination divided by step size gives number of steps needed
	double step_limit = m_goal_range/sim_speed/m_step;

        double min_range = m_range;
						  
	for (int i=1; i<step_limit; i++) {
	  //IncrementStep(m_step);
          contact_x = contact_x + m_step * contact_spd * cos(contact_angle*MPI/180);
          contact_y = contact_y + m_step * contact_spd * sin(contact_angle*MPI/180);
          own_x = own_x + m_step * sim_speed * cos(m_goal_heading*MPI/180);
          own_y =  own_y + m_step * sim_speed * sin(m_goal_heading*MPI/180);
	 
	  double dis_x = contact_x - own_x;
	  double dis_y = contact_y - own_y;
	  double range = sqrt((dis_x*dis_x)+(dis_y*dis_y));
 	  if (range < min_range){
	    min_range = range;
	   }
	  if (range < min_cpa){
	    min_cpa = range;
	    closest = vname;
	  }
        }//min_range is now the small range between contact and ownship 

       if (min_range<m_range){
	  counter++;
        }      
      }
      
     m_map_density_count[speed_str]=counter;
     m_map_min_range[speed_str] = min_cpa;
     m_map_closest_contact[speed_str] = closest;
   }
 
 }

//----------------------------------------------------------
#if 0
// Check to see if a single contact is in range

bool DensityCounter::InRange(double m_range)
{
  bool in_range = false;
  calculateGoal();
  double step_limit = m_goal/m_own_speed/m_step;
  //Distance to goal divided by speed of own ship gives time to destination.
  //Time to distination divided by step size gives number of steps needed
  for (int i=1; i<step_limit; i++) {
   // IncrementStep(m_step);
    
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

#endif 
//-------------------------------------------------------------
// Process noderecord class
// process node contact and add contact
void DensityCounter::AddContact(NodeRecord m_record)
  
{
    string vname = m_record.getName();
    m_map_contact_x[vname] =  m_record.getX();
    m_map_contact_y[vname] =  m_record.getY();
    m_map_contact_heading[vname] =  m_record.getHeading();
    m_map_contact_speed[vname] =  m_record.getSpeed();
    //m_contact_name = m_record.getName();
}

//----------------------------------------------------------
// Calculate distance and heading  to goal
void DensityCounter::calculateGoal ()
 {
    double dis_x = m_goal_x - m_own_x;
    double dis_y = m_goal_y - m_own_y;
    m_goal_range =  sqrt((dis_x*dis_x)+(dis_y*dis_y));
    m_goal_heading = atan2 (dis_y, dis_x) ;//radians
}

//---------------------------------------------------------
 double DensityCounter::HeadingToAngle(double heading)
 {
   double angle =  90 - heading; 
   return(angle);
 }
//---------------------------------------------------------
//Procedure: setOwnShip()
//This procedure sets ownship values for density counter objects
void DensityCounter::setOwnship(double x, double y, double hdg, double spd)
{
  setX(x);
  setY(y);
  setHeading(hdg);
  setSpeed(spd);
}
//---------------------------------------------------------
//Procedure: setGoal()
//This procedure sets goal for density counter objects
void DensityCounter::setGoal(double goal_x, double goal_y)
{
  setGoalX(goal_x);
  setGoalY(goal_y);
}

//-------------------------------------------------------------
string DensityCounter::getReport()
{
  string x = doubleToStringX(m_own_x);
  string y = doubleToStringX(m_own_y);
  string hdg = doubleToStringX(m_own_heading);
  string spd = doubleToStringX(m_own_speed);

  string range = doubleToStringX(m_range);

  string report = "own ship x, y, hdg, spd: " + x +", " + y + ", " + hdg +"," + spd;
  //report += ", " + m_contact_name + "'s  x, y, hdg, spd: " + c_x +", " +c_y + ", " +c_hdg +"," +c_spd;
  report += "range limit: " + range;

  return(report);
}


//-------------------------------------------------------------
ACTable DensityCounter::getDetailedReport()
{
    
 ACTable actab(4,5);
  actab.setColumnJustify(1, "right");
  actab << "Speed | Peak Density | Closest | min CPA";
  actab.addHeaderLines();

  map<string, int>::iterator p;
  for(p=m_map_density_count.begin(); p!=m_map_density_count.end(); p++) {
    string speed_str = p->first;
    double contact_count = p->second;
    string count_str = intToString(contact_count);
    string closest_contact = m_map_closest_contact[speed_str];
    string closest_cpa=  doubleToStringX(m_map_min_range[speed_str]);
    actab<<speed_str<<count_str<<closest_contact<<closest_cpa;
   }

  return(actab);
}

 //-------------------------------------------------------------
ACTable DensityCounter::getContacts()
{
    
  ACTable actab(5,5);
  actab.setColumnJustify(1, "right");
  actab << "Vname | Contact X | Contact Y | Heading | Speed";
  actab.addHeaderLines();

  map<string, double>::iterator p;
  for(p=m_map_contact_x.begin(); p!=m_map_contact_x.end(); p++) {
    string vname = p->first;
    double contact_x = p->second;
    string x_str =doubleToStringX(contact_x);
    string y_str = doubleToStringX(m_map_contact_y[vname]);
    string hdg_str=  doubleToStringX(m_map_contact_heading[vname]);
    string spd_str=  doubleToStringX(m_map_contact_speed[vname]);
    actab<<vname << x_str << y_str << hdg_str;
   }

  return(actab);
}
