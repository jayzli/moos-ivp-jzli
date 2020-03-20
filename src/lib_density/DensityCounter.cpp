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
  m_own_y =0;
  m_own_heading=0;
  m_own_speed=0;

  m_contact_x=0 ;
  m_contact_y=0;
  m_contact_heading=0;
  m_contact_speed=0;
  
  m_density = 0;
}

//---------------------------------------------------------
// Destructor

DensityCounter::~DensityCounter()
{
  
}

//----------------------------------------------------------
// Handles a single contact

bool DensityCounter::InRange(double m_range)
{
  bool in_range = false;
  //put this inside loop
  //increasement to new positions
  //calculate distance between ownship and contact
  //if distance less than m_range, return true
  
    
  return (in_range);
 }
