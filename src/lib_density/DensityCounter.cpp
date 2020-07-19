/*****************************************************************/
/*    NAME: John Li                                              */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DensityCounter.cpp                                   */
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
  m_max_speed = 5; //max speed meters per second
  
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
  //operations that only needs to be done once per tick
  calculateGoal(); // calculate range and heading to goal
  cout<<"goal is "<<"x="<<m_goal_x<<" y="<<m_goal_y<<endl;
  cout<<"distance to goal is "<<m_goal_range<<" angle to goal is "<<m_goal_heading<<endl;
  
  //first for loop goes considers all possible speed 
  for (double j=1; j<=m_max_speed*10.01; j++){
      double sim_speed = j/10; //simulated speed go from .1 to max speed 
      string speed_str = doubleToStringX(sim_speed);
      int peak_density = 0; // 
      double min_cpa = m_range;//this is the minimum cpa for this particular speed for any of the vehicles
      string closest = "no vessel in range";//store name of closest vessel

      cout<<"*****speed in consideration is "<<speed_str<<endl;

      //Distance to goal divided by simulated speed of own ship gives time to destination.
      //Time to distination divided by step size gives number of steps needed
      double step_limit = m_goal_range/sim_speed/m_step;

      double own_x = m_own_x;
      double own_y = m_own_y;
      //cout<<"ship x and y are: "<<own_x<<" "<<own_y<<endl;

      //define working maps
      map<string, double> map_contact_x=m_map_contact_x;
      map<string, double> map_contact_y=m_map_contact_y;
      map<string, double> map_contact_heading=m_map_contact_heading;
      map<string, double> map_contact_speed=m_map_contact_speed;
      map<string, double> map_contact_range=m_map_contact_range;
      
      //second loop steps forward in time for simulation
      for (int i=1; i<step_limit; i++) {
	cout<<"*****************step "<<i<<"**********************"<<endl;

	own_x = own_x + m_step * sim_speed * cos(m_goal_heading*MPI/180);
	own_y =  own_y + m_step * sim_speed * sin(m_goal_heading*MPI/180);
        cout<<"ship x and y are: "<<own_x<<" "<<own_y<<endl;
	
	double min_range = m_min_range;//this is the minimum range for a particular time slot
	int density_count = 0;
	
        //third loop goes through all contacts tracked by the class
	map<string, double>::iterator q;	
	for (q=map_contact_x.begin(); q!=map_contact_x.end();q++){
	  string      vname = q->first;
	  double  contact_x = q->second;
	  double  contact_y = map_contact_y[vname];
	  double  contact_hdg = map_contact_heading[vname];
	  double  contact_spd = map_contact_speed[vname];					  
	  double  contact_angle = HeadingToAngle(contact_hdg);

	  //IncrementStep(m_step);
          contact_x = contact_x + m_step * contact_spd * cos(contact_angle*MPI/180);
          contact_y = contact_y + m_step * contact_spd * sin(contact_angle*MPI/180);
          cout<<"contact "<<vname<<" x and y are: "<<contact_x<<" "<<contact_y<<endl;
	  
	  //Save new contact info to working map
	  map_contact_x[vname]=contact_x;
	  map_contact_y[vname]=contact_y;
	  
	  //calculate range between contact vname and own ship
	  double dis_x = contact_x - own_x;
	  double dis_y = contact_y - own_y;
	  double range = sqrt((dis_x*dis_x)+(dis_y*dis_y));
          cout<<"calculated range is "<<range<<endl;
	  
	  if (i == 1){
	     map_contact_range[vname]=range;
	  }

	  //find the minimum range less than set range 
 	  if (range < min_range){
	    min_range = range;
	    closest = vname;
	   }

	  if (min_range < min_cpa){
	    min_cpa=min_range;
	  }
	  
	  //if any ship is within min_cpa set 
	  if (range < m_range){
	    density_count++;	    
	  }

	  //removed this block to simulate further even if contact opens
	  #if 0
      	  //once range start opening, stop simulating forward
	  if (i != 1 && range > map_contact_range[vname]){
	    cout<<vname<<" is opening at step "<<i<<endl;
	    q = map_contact_x.erase(q);
	  } else{
	    q++;
	    map_contact_range[vname]=range;
	  }
	  #endif

	  cout<<"----------------------------------------------"<<endl;
        }//end of third for loop

	if (peak_density < density_count){
	  peak_density = density_count;
	}

	cout<<"min range for time step "<<i<< " is "<<min_range<<endl;
	cout<<"Number of contact in range is "<<density_count<<endl;
	//min_range is now the small range between contact and ownship 

        //break out of loop if all contacts are out
        if (map_contact_x.empty()) {
         break;
        }
	cout<<"**********************************************"<<endl;
      }//end of second for loop

     cout<<"min_range for this speed is" <<min_cpa<<endl;
    
     m_map_density_count[sim_speed]=peak_density;
     m_map_min_range[sim_speed] = min_cpa;
     m_map_closest_contact[sim_speed] = closest;

     cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
  }//end of first for loop
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
    // cout<<"added contact"<<vname<<flush;
}

//----------------------------------------------------------
// Calculate distance and heading  to goal
void DensityCounter::calculateGoal ()
 {
    double dis_x = m_goal_x - m_own_x;
    double dis_y = m_goal_y - m_own_y;
    m_goal_range =  sqrt((dis_x*dis_x)+(dis_y*dis_y));
    m_goal_heading = atan2 (dis_y, dis_x) ;//radians
    cout<<"goal range calculated"<<endl<<flush;
}

//---------------------------------------------------------
 double DensityCounter::HeadingToAngle(double heading)
 {
   double angle;
   if (heading <=90){
     angle =  90 - heading;
     }
   if (heading > 90){
     angle = 450 - heading;
     }
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
  cout<<"ownship set"<<endl<<flush;
}
//---------------------------------------------------------
//Procedure: setGoal()
//This procedure sets goal for density counter objects
void DensityCounter::setGoal(double goal_x, double goal_y)
{
  setGoalX(goal_x);
  setGoalY(goal_y);
  cout<<"goal set"<< endl<<flush; 
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
  report += " range limit: " + range;

  return(report);
}


//-------------------------------------------------------------
ACTable DensityCounter::getDetailedReport()
{
    
 ACTable actab(4,5);
  actab.setColumnJustify(1, "right");
  actab << "Speed | Peak Density | Closest | min CPA";
  actab.addHeaderLines();

  map<double, int>::iterator p;
  for(p=m_map_density_count.begin(); p!=m_map_density_count.end(); p++) {
    double speed = p->first;
    double contact_count = p->second;
    string speed_str = doubleToStringX(speed); 
    string count_str = intToString(contact_count);
    string closest_contact = m_map_closest_contact[speed];
    string closest_cpa =  doubleToStringX(m_map_min_range[speed]);
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
    actab<<vname << x_str << y_str << hdg_str <<spd_str;
   }

  return(actab);
}

        
 //-------------------------------------------------------------
string DensityCounter::getDensities()
{
    
  string densities;

  map<double, int>::iterator p;
  for(p=m_map_density_count.begin(); p!=m_map_density_count.end(); p++) {
    double speed = p->first;
    int density = p->second;
    string speed_str =doubleToString(speed);
    string density_str = intToString(density);
    densities += speed_str;
    densities += ":";
    densities += density_str;
    densities += ",";
   }

  return(densities);
}

 //-------------------------------------------------------------
string DensityCounter::getUtilities()
{
    
  string utilities;

  map<double, int>::iterator p;
  for(p=m_map_density_count.begin(); p!=m_map_density_count.end(); p++) {
    double speed = p->first;
    int density = p->second;   
    string speed_str =doubleToStringX(speed);
    int utility = 100 - 20 * density;
    string utility_str = intToString(utility);
    utilities += speed_str;
    utilities += ":";
    utilities += utility_str;
    utilities += ",";
   }

  return(utilities);
}
        
