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

  double m_min_range = 10000; 

  m_step = 1;  //in seconds
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

int DensityCounter::returnCount(double range, double step)
{
  m_range = range;
  m_step = step;
  

  for (int j=1; j<m_speed_range; j++){
      ChangeSpeed(j);
      string speed_str = doubleToStringX(m_nav_spd);
      int counter=0;
      string vname="no vessel in range";
      double min_cpa=m_range;
      
      // check to see if this particular contact will be in range 
      map<string, DensityCounter>::iterator q;
      for (q=m_map_density_counter.begin(); q!=m_map_density_counter.end(); q++){
        string     contact_name = q->first;
        DensityCounter density_counter  = q->second;

	//SetOwnShip(contact_name);  wrong place to set ship name
        m_report=density_counter.getReport();
	
	double cpa = density_counter.calRange();

	if (cpa<m_range){
	  counter++;
	  }
	if (cpa<min_cpa){
	  vname=contact_name;
	  min_cpa=cpa;
	  }
	
	//m_map_density_reports[speed_str]=density_counter.getReport();

	
	//Notify("DENSITY_REPORT", m_report);
        }
	//record result for this particulat speed

       m_map_contact_count[speed_str]=counter;
       m_map_closest_contact[speed_str]=vname;
       m_map_closest_CPA[speed_str]=min_cpa;

  }
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
// Calculate distance to goal
void DensityCounter::calculateGoal ()
 {
    double dis_x = m_goal_x - m_own_x;
    double dis_y = m_goal_y - m_own_y;
    m_goal =  sqrt((dis_x*dis_x)+(dis_y*dis_y));
}


//---------------------------------------------------------

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
  actab << "Vname | Contact X | Contact Y | Range | In or Not";
  actab.addHeaderLines();

  map<string, int>::iterator p;
  for(p=m_map_contact_x.begin(); p!=m_map_contact_x.end(); p++) {
    string vname = p->first;
    double contact_x = p->second;
    string x_str =doubleToStringX(contact_x);
    string y_str = doubleToStringX(m_map_contact_y[vname]);
    string range_str=  doubleToStringX(m_map_contact_range[vname]);
    actab<<vname << x_str << y_str << range_str;
   }
  m_msgs << actab.getFormattedString();

  return(actab);
}
