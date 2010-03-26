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
 * The source file for pid_class.
 */

#include "pid_class.h"
#include "ahns_logger.h"

/**
 * Initialisation of one PID loop.
 * @param P Proportional gain.
 * @param I Integral gain.
 * @param D Derivative gain.
 */
ahnsPID::ahnsPID(double P, double I, double D)
{
	k_p = P;
	k_i = I;
	k_d = D;

	error = 0;
	prev_error = 0;
	prev_time = 0;
	integral = 0;

	// set it to virtually undbounded. these values are big. and i mean really big.  if they are not big enough for you then you need to start using smaller numbers. you may think it's a long way down to the local chemist, but that's peanuts compared to space.
	upper_bound = 1e23;
	lower_bound = -1e23;
}

/**
 * Function to reassign gains
 * @param P Proportional gain.
 * @param I Integral gain.
 * @param D Derivative gain.
 */
void ahnsPID::setGains(double P, double I, double D)
{
	k_p = P;
	k_i = I;
	k_d = D;
}

/**
 * Sets the boundaries (saturation values) for the PID controller.  Currently this just limits all three components to the values specified.
 * @param upper The upper limit.
 * @param lower The lower limit.
 */
void ahnsPID::setBounds(double upper, double lower)
{
	upper_bound = upper;
	lower_bound = lower;
}

/**
 * PID loop function. Run each iteration of the loop. 
 * @note If the time passed is the same or less than the previous time, an integer exception is thrown.
 * @param Desired 	The command value.
 * @param Current 	The current state value.
 * @param Time 		The time corresponding to the current state value.
 * @param 		*components A pointer to a three element double array in which the individual contributions of proportional (P), integral (I) and derivative (D) control are stored, for informative purposes (e.g. plotting or debugging, no functional purpose).  If NULL then the pointer isn't used.  This argument is optional.
 * @return 		The output of the PID controller, the command.
 */
double ahnsPID::run(double desired, double current, double time, double *components)
{
	double delta_t = time-prev_time;
	prev_time = time;
	double error = desired - current;
	double P, I, D;

	AHNS_DEBUG("ahnsPID::run()");
	AHNS_DEBUG("run:: the command: ( " << desired << " ) the current: ( " << current << " ) the error: ( " << error << " )");
	AHNS_DEBUG("run:: delta_t: ( " << delta_t << " )");

	if(delta_t <= 0)
	{
		throw BAD_TIME;
	}

	P = k_p*error;
	if(P > upper_bound)
		P = upper_bound;
	else if(P < lower_bound)
		P = lower_bound;

	if(k_i!=0.0)
	{
		integral+= delta_t*(prev_error + error)/2;

		if(integral > upper_bound/k_i)
			integral = upper_bound/k_i;
		else if(integral < lower_bound/k_i)
			integral = lower_bound/k_i;

		I = k_i*integral;
	}
	else
	{
		integral = 0;
		I = 0;
	}

	D = k_d*(error-prev_error)/delta_t;

	if(D > upper_bound)
		D = upper_bound;
	else if(D < lower_bound)
		D = lower_bound;

	AHNS_DEBUG("run:: P: ( " << P << " ) I: ( " << I << " ) D: ( " << D << " ) ");

	if(components != NULL)
	{
		components[0] = P;
		components[1] = I;
		components[2] = D;
	}

	prev_error = error;

	return P+I+D;
}

