/************************************************************/
/*    NAME: John Li                                 */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                    */
/*    DATE: 20 FEB 20                                       */
/************************************************************/

#ifndef PRIMEENTRY_HEADER
#define PRIMEENTRY_HEADER 

#include <vector>
#include <cstdlib>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <stdlib.h>

using namespace std;

class PrimeEntry {
public: 
	//PrimeEntry();
	PrimeEntry();
	~PrimeEntry();
	void PrimeFinder();
	void setOriginalVal(unsigned long int v);
	void setArriveTime(long double v);
	void setReceivedIndex(unsigned int v) {
		m_received_index = v;
	};
	void setCalculatedIndex(unsigned int v) {
		m_calculated_index = v;
	};
	void setCalculatedTime(long double v);
	string getReport();

public:
	uint64_t		 			m_start_index;
	uint64_t                    m_counter;
	uint64_t					m_orig;
	uint64_t					m_temp_num;
	int 						m_num_steps;
	unsigned int 				m_received_index;
	unsigned int 				m_calculated_index;
	bool 			 			m_done;
	vector<uint64_t>		 	m_factors;
	string 						m_report_out;
	double						m_time_arrive;
	double						m_time_calculated;
	double						m_total_time;
};

#endif
