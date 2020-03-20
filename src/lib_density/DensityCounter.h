/*****************************************************************/
/*    NAME: John Li                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DensityCounter.cpp                                        */
/*    DATE: Mar 19th 2020                                        */
/*****************************************************************/
 
#ifndef DensityCounter_HEADER
#define DensityCounter_HEADER

#include <string>

using namespace std;

class DensityCounter
{
 public:
  DensityCounter();
  ~DensityCounter() {};

  
 protected:
  bool InRange(double m_range);
 
 protected: // State variables
  double  m_own_x ;
  double  m_own_y;
  double  m_own_heading;
  bool    m_own_speed;

  double  m_contact_x ;
  double  m_contact_y;
  double  m_contact_heading;
  bool    m_contact_speed;
  
  double m_range;
  
  int m_density; 
};

#endif 

