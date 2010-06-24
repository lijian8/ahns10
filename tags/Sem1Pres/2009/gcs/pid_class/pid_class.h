/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-10-18 00:05:16 +1000 (Sun, 18 Oct 2009) $
 * $Rev: 860 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * The header file for %pid_class.
 */
#ifndef AHNS_PID
#define AHNS_PID

#include <stdlib.h>
#define BAD_TIME 2

/** 
 * Class defining a PID controller for the AHNS project.  The class is initialised with the three gains in %ahnsPID::ahnsPID() and then run at every iteration with ahnsPID::run().  The gains can be changed at any time with ahnsPID::setGains().
 */
class ahnsPID
{
public:
	ahnsPID(double P, double I, double D);
	void setGains(double P, double I, double D);
	void setBounds(double upper, double lower); 
	double run(double desired, double current, double time, double* components = NULL);
	
private:

	/// Proportional gain
	double k_p;
	/// Integral gain
	double k_i;
	/// Derivative gain
	double k_d;	
	
	/// Error between state and command
	double error; 	
	/// Error in previous iteration
	double prev_error;	
	/// Time at previous iteration
	double prev_time;
	/// Variable used for integration
	double integral;

	/// Upper limit for PID components
	double upper_bound;

	/// Lower limit for PID components
	double lower_bound;
};

#endif 

