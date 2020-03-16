/************************************************************/
/*    NAME: John Li                                    */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                  */
/*    DATE: 20 FEB 20                                       */
/************************************************************/

#include "PrimeEntry.h"

using namespace std;


PrimeEntry::~PrimeEntry() {
}

PrimeEntry::PrimeEntry() {
  m_num_steps = 0;
  m_orig = 0;
  m_done = false;
  m_start_index = 2;
  m_received_index = 0;
  m_calculated_index = 0;
}

void PrimeEntry::setOriginalVal(unsigned long int v) {
	m_orig = v;
	m_temp_num = m_orig;
}

void PrimeEntry::setArriveTime(long double v) {
	m_time_arrive = v;
}

void PrimeEntry::setCalculatedTime(long double v) {
	m_time_calculated = v;
	m_total_time = m_time_calculated - m_time_arrive;
}

void PrimeEntry::PrimeFinder() 
{		  
	//Factorization
	for (int j=m_start_index; j<=sqrt(m_temp_num); j++) {	
		while (m_temp_num % j == 0) {							//Check if m_temp_num is divisible by "j"
			m_temp_num = m_temp_num/j;
			m_factors.push_back(j);								//Store value of "j" into factors vector
			m_num_steps++;										//Determine number of iterations					
		}
		m_num_steps++;
		m_start_index++;
	}	
	
	//After Factorization is complete
	if (m_temp_num != 1) {
		m_factors.push_back(m_temp_num);
		m_done = true;
	}
	else 
		m_done = true;

	return;
}

string PrimeEntry::getReport()
{
	stringstream m_primes;
	string m_primes_str;

	for (int i = 0; i<m_factors.size()-1;i++) {							//Concatenate prime factors into string, separated by a ":"
		m_primes << m_factors[i] << ":";
		m_primes_str = m_primes.str();
	}

	for (int i = m_factors.size()-1; i<m_factors.size(); i++) {			//Remove the ":" from the end of string
		m_primes << m_factors[i];								
		m_primes_str = m_primes.str();
	}
	
	stringstream m_combined;											//Combine output into a single report
	m_combined << "orig=" << m_orig << ",received=" << m_received_index << ",calculated=" << m_calculated_index << ",solve_time=" << m_total_time << ",primes=" << m_primes_str << ",username=conwayrl" << endl;
	m_report_out = m_combined.str();

	return(m_report_out);
}
