/**
 * @file
 * @author Alex Wainwright, Owen Plagens
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-10-18 00:05:16 +1000 (Sun, 18 Oct 2009) $
 * $Rev: 860 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * The autopilot.
 */

/*psuedocode:

> TODO: scale raw data from telemetry.
> state estimator takes values from telemetry and estimates state.
> controller takes state estimated by state estimator and calculates commands.
> outputs sent to groundpulse

*/

#include "ahns_autopilot.h"

#include <QThread>
#include <QTime>

#include <iostream>

#include <cmath>

#include "ahns_logger.h"

#include "newmat/newmatio.h"

#define TELEMETRY_CLOCK_SCALE (80000.0)

using namespace std;
using namespace uplink;
using namespace NEWMAT;

/**
 * Initialises the autopilot thread
 * @param *heli A pointer to the telemetry structure for the particular UAV
 * @param serial_port The serial port for the link to groundpulse - the uplink
 */
ap_thread::ap_thread(Telemetry *heli,int serial_port, heli_trims *h_trims)
{
	AHNS_DEBUG("ap_thread::ap_thread()");
	m_telemetry = heli;
	groundpulse_port = serial_port;
	trim_vals = h_trims;

	AHNS_DEBUG("ap_thread:: initialising PIDs");
	pitch2long = new ahnsPID(0.0, 0.0, 0.0);
	roll2lat = new ahnsPID(0.0, 0.0, 0.0);
	yaw2rud = new ahnsPID(0.0, 0.0, 0.0);
	alt2col = new ahnsPID(0.0,0.0,0.0);
	
	AHNS_DEBUG("ap_thread:: initialising state estimator");
	init_state_estimator();
	AHNS_DEBUG("ap_thread:: constructor finished successfully");
}

/**
 * Returns the latest state as calculated by the state estimator.
 */
State ap_thread::get_state()
{
	return live_state;
}

/** 
 * Gets the values of the individual P, I and D components in the control loops
 * @param lat A pointer to a 3 element double array, in which the P, I and D gains for the lateral control loop will be saved.
 * @param lon As above for longitudinal control 
 * @param col As above for collective control
 * @param rud As above for rudder control
 */
void ap_thread::getPIDs(double* lat, double* lon, double* col, double* rud)
{
	int ii;
	for(ii=0; ii<3; ii++)
	{
		lat[ii] = latPID_vals[ii];
		lon[ii] = lonPID_vals[ii];
		rud[ii] = rudPID_vals[ii];
		col[ii] = colPID_vals[ii];
	}
}

/**
 * Indicates which control channels are saturated.
 * @param *latSat A pointer to an integer which will be set to indicate the saturation state of the lateral control using macros defined in
 ahns_autopilot.h
 * @param *lonSat A pointer to an integer which will be set to indicate the saturation state of the longitudinal control using macros defined in ahns_autopilot.h
 * @param *colSat A pointer to an integer which will be set to indicate the saturation state of the collective control using macros defined in ahns_autopilot.h
 * @param *rudSat A pointer to an integer which will be set to indicate the saturation state of the rudder control using macros defined in ahns_autopilot.h
 */
void ap_thread::querySaturation(int* latSat, int* lonSat, int* colSat, int* rudSat)
{
	*latSat = m_latSat;
	*lonSat = m_lonSat;
	*colSat = m_colSat;
	*rudSat = m_rudSat;
}

/**
 * Returns the latest commands as created by the controller.
 */
heli_command ap_thread::get_cmd()
{
	return live_cmd;
}

/**
 * Set the autopilot gains.
 * @param *pitch A pointer to a three element array of doubles corresponding to the values of the P, I and D gains for the pitch2long loop.
 * @param *roll As above but for the roll2lat loop.
 * @param *yaw As above but for the yaw2rud loop.
 * @param *collective As above but for the alt2col loop.
 */
void ap_thread::setGains(double *pitch, double *roll, double *yaw, double *collective)
{ //TODO: test that this works
	AHNS_DEBUG("ap_thread::setGains()");

	pitch2long->setGains(pitch[0], pitch[1], pitch[2]);
	roll2lat->setGains(roll[0], roll[1], roll[2]);
	yaw2rud->setGains(yaw[0], yaw[1], yaw[2]);
	alt2col->setGains(collective[0], collective[1], collective[2]);

	AHNS_DEBUG("ap_thread::setGains:: pitch gains set to: \t\t( " 
		<< fixed << setprecision(3) << pitch[0] << ", " 
		<< pitch[1] << ", " 
		<< pitch[2] << " )");
	AHNS_DEBUG("ap_thread::setGains:: roll gains set to: \t\t( " 
		<< setprecision(3) << roll[0] << ", " 
		<< roll[1] << ", " 
		<< roll[2] << " )");
	AHNS_DEBUG("ap_thread::setGains:: yaw gains set to: \t\t( " 
		<< setprecision(3) << yaw[0] << ", " 
		<< yaw[1] << ", " 
		<< yaw[2] << " )");
	AHNS_DEBUG("ap_thread::setGains:: collective gains set to: \t( " 
		<< setprecision(3) << collective[0] << ", " 
		<< collective[1] << ", " 
		<< collective[2] << " )");
}

/**
 * Set the autopilot commands.
 * @param pitch The pitch command
 * @param roll The roll command
 * @param yaw The yaw command
 * @param alt The altitude command
 */
void ap_thread::setCommands(double pitch, double roll, double yaw, double alt)
{
	AHNS_DEBUG("ap_thread::setCommands()");
	pitch_cmd = pitch;
	roll_cmd = roll;
	yaw_cmd = yaw;
	altitude_cmd = alt;
	AHNS_DEBUG("ap_thread::setCommands:: commands set to\n\tPitch:\t( " << pitch << " )\n\tRoll:\t( " << roll << " )\n\tYaw:\t( " << yaw << " )\n\tAlt:\t( " << alt << " )");
}

/** 
 * Set the autopilot boundaries
 * @param lon_max The upper boundary for longitudinal control
 * @param lon_min The lower boundary for longitudinal control
 * @param lat_max The upper boundary for lateral control
 * @param lat_min The lower boundary for lateral control
 * @param col_max The upper boundary for collective control
 * @param col_min The lower boundary for collective control
 * @param rud_max The upper boundary for rudder control
 * @param rud_min The lower boundary for rudder control
 */
void ap_thread::setBounds(double lon_max, double lon_min, double lat_max, double lat_min, double col_max, double col_min, double rud_max, double rud_min)
{
	AHNS_DEBUG("ap_thread::setBounds()");
	
	alt2col->setBounds(col_max, col_min);
	pitch2long->setBounds(lon_max, lon_min); 
	roll2lat->setBounds(lat_max, lat_min);
	yaw2rud->setBounds(rud_max, rud_min);
}

/**
 * The autopilot thread destructor
 */
ap_thread::~ap_thread()
{
	if (pitch2long != NULL)
	{
		delete pitch2long;
		pitch2long = NULL;
	}
	if (roll2lat != NULL)
	{
		delete roll2lat;
		roll2lat = NULL;
	}
}

/**
 * The main autopilot thread. This function runs once per cycle of the control system.  In one iteration it takes the latest telemetry data, calls the state estimator (Estimator()), and then in turn calls the controller (Controller()).  The controller sends commands to the helicopter. 
 */
void ap_thread::run()
{
	QTime t, t2;
	t.start();
	static State s_emily;
	int ret; 
	
	AHNS_DEBUG("ap_thread::run()");
	AHNS_DEBUG("ap_thread::run:: roll_rate: (" << setbase(16) << m_telemetry->roll_rate << ")\ttelemetry time ( " << m_telemetry->time << " )");
	
	ret = Estimator(m_telemetry, &s_emily);
	AHNS_DEBUG("ap_thread::run:: Time for estimator to run: ( " << t.elapsed() << " ) ");

	t2.start();
	ret = Controller(&s_emily);
	AHNS_DEBUG("ap_thread::run:: Time for autopilot to run: ( " << t2.elapsed() << " ) ");
	
}

/** 
 * State Estimator
 * The state estimator for the autopilot.
 *
 * pre:: 	valid telemetry data
 *
 * post:: 	sets structure at state to estimated state values
 *		returns integer value indicating success of function 
 *
 * @param *telem A pointer to the telemetry structure representing data from the helicopter. The input to the state estimator.
 * @param *state A pointer to the state structure representing the helicopter. The output of the state estimator.
 */
int ap_thread::Estimator(Telemetry *telem, State *state)
{
	AHNS_DEBUG("ap_thread::Estimator()");
	AHNS_DEBUG("ap_thread::Estimator:: telems->roll_rate: (" << telem->roll_rate << ")");
	
	static double prev_time = 0;
	AHNS_DEBUG("ap_thread::Estimator:: telem->time ( " << setbase(10) << telem->time << " ) ");
	double delta_t = (telem->time - prev_time)/TELEMETRY_CLOCK_SCALE;
	AHNS_DEBUG("ap_thread::Estimator:: time ( " << telem->time << " )\t prev_time ( " << prev_time << " )\t delta_t ( " << delta_t << " ).");
	prev_time = telem->time;
	double corrected_alt, alt_voltage, corrected_batt;

	state->time = telem->time/TELEMETRY_CLOCK_SCALE;// magic numbers
	AHNS_DEBUG("ap_thread::Estimator:: state time: ( " << state->time << " )");
	
/*	state->x = telem->x_accel; //TODO: THIS IS OBVIOUSLY WRONG MAN
	state->y = telem->y_accel;
	state->z = telem->z_accel;
	state->x_rate = 0;
	state->y_rate = 0;
	state->z_rate = 0;
	state->yaw = 0;
	state->pitch = 0;
	state->roll = 0;
	state->yaw_rate = 0;
	state->pitch_rate = 0;
	state->roll_rate = 0;*/

	AHNS_DEBUG("ap_thread::Estimator:: update A matrix");
	(*A)(1,2) = -delta_t;
	(*A)(3,4) = -delta_t;
	AHNS_DEBUG("ap_thread::Estimator:: A matrix ( \n" << *A << "\n ).");
	
	AHNS_DEBUG("ap_thread::Estimator:: update B matrix");
	(*B)(1,1) = delta_t;
	(*B)(3,2) = delta_t;
	AHNS_DEBUG("ap_thread::Estimator:: B matrix ( \n" << *B << "\n ).");

	AHNS_DEBUG("ap_thread::Estimator:: update u matrix");
	*u << telem->roll_rate << telem->pitch_rate;
	AHNS_DEBUG("ap_thread::Estimator:: u matrix ( \n" << *u << "\n ).");
	AHNS_DEBUG("ap_thread::Estimator:: update x matrix");
	*x = *A * *x + *B * *u;
	AHNS_DEBUG("ap_thread::Estimator:: updated x matrix ( \n" << *x << "\n ).");
	AHNS_DEBUG("ap_thread::Estimator:: update y matrix");
	*y << atan2(telem->y_accel,telem->z_accel) << 0 << atan2(telem->x_accel,telem->z_accel) << 0;
	AHNS_DEBUG("ap_thread::Estimator:: y matrix ( \n" << *y << "\n ).");
	
	AHNS_DEBUG("ap_thread::Estimator:: update s, K, x, P matrices");
	*s = *C * *P * (*C).t() + *S_z;
	*K = *A * *P * (*C).t() * (*s).i();
	*x = *x + *K * (*y - *C * *x);
	*P = *A * *P * (*A).t() - *K * *C * *P * (*A).t() + *S_w;

	AHNS_DEBUG("ap_thread::Estimator:: output roll and pitch states");
	state->roll = (*x)(1, 1);
	state->pitch = (*x)(3, 1);
	AHNS_DEBUG("ap_thread::Estimator:: roll ( "<< state->roll <<" )\t pitch ( " << state->pitch << " ). ");
	AHNS_DEBUG("ap_thread::Estimator:: x_accel ( "<< telem->x_accel <<" )\t y_accel ( " << telem->y_accel << " )\t z_accel ( " << telem->z_accel << " ). ");
	AHNS_DEBUG("ap_thread::Estimator:: roll_rate ( "<< telem->roll_rate <<" )\t pitch_rate ( " << telem->pitch_rate << " )\t yaw_rate ( " << telem->yaw_rate << " ). ");

	
	alt_voltage = 2.56*telem->alt/1023;
	corrected_alt = 1 / (0.0073 * alt_voltage - 0.008); // this is a relationship built on trust and understanding... mostly trust.
//	prev_alt = *alt_buff_head;  // save value before replacing with new value.
	//state->z = corrected_alt * cos(state->roll) * cos(state->pitch);

	corrected_batt = telem->battery*100/(BATT_MAX-BATT_MIN) - 100/(BATT_MAX-BATT_MIN)*BATT_MIN;
	if(corrected_batt > 100)
		corrected_batt = 100;
	else if(corrected_batt < 0)
		corrected_batt = 0;
	state->battery = batt_av->update(corrected_batt);
	AHNS_DEBUG("ap_thread::Estimator: the telemetry battery value ( " << telem->battery << " ) ");
	AHNS_DEBUG("ap_thread::Estimator: the state battery value ( " << state->battery << " )");

	state->z = alt_av->update(corrected_alt);
//	state->z = corrected_alt;
	state->period = period_av->update(delta_t); // in seconds

	AHNS_DEBUG("ap_thread::Estimator:: altitude ( "<< telem->alt << " ) voltage( " << alt_voltage <<" )\t corrected altitude ( " << state->z << " ) ");

	live_state = *state;
	emit new_state();
	
	return 0;
}

/**
 * A function to change the length of the altitude signal low pass filter.
 * @param input The new length of the filter
 */
void ap_thread::setAltFiltLength(int input)
{
	AHNS_DEBUG("ap_thread::setAltFiltLength()");
	AHNS_DEBUG("ap_thread::setAltFiltLength:: setting filter length to: ( " << input << " )");
	alt_av->setLength(input);
}

/**
 * A function to change the length of the packet period low pass filter.
 * @param input The new length of the filter
 */
void ap_thread::setPeriodFiltLength(int input)
{
	period_av->setLength(input);
}

/**
 * A function to change the length of the battery level low pass filter.
 * @param input The new length of the filter
 */
void ap_thread::setBattFiltLength(int input)
{
	batt_av->setLength(input);
}
/**
 * Initialise the state estimator - allocates memory, sets up filters and initialises variables.
 */
void ap_thread::init_state_estimator()
{
	AHNS_DEBUG("ap_thread::init_state_estimator()");
	int ii;

	// should probably initialise in the ap_thread constructor (the length)
	alt_av = new moving_average(10, 0);
	period_av = new moving_average(10, 0);
	batt_av = new moving_average(10, 50);

	AHNS_DEBUG("ap_thread::init_state_estimator:: initialising matrices");
	A = new Matrix(4, 4);
	B = new Matrix(4, 2);
	C = new Matrix(4, 4);
	x = new Matrix(4, 1);
	u = new Matrix(2, 1);
	y = new Matrix(4, 1);
	P = new Matrix(4, 4);
	s = new Matrix(4, 4);
	S_z = new Matrix(4, 4);
	S_w = new Matrix(4, 4);
	K = new Matrix(4, 4);
	
	*A = 0.0;
	*B = 0.0;
	*C = 0.0;
	*x = 0.0;
	*u = 0.0;
	*y = 0.0;
	*P = 0.0;
	*s = 0.0;
	*S_z = 0.0;
	*S_w = 0.0;
	*K = 0.0;
	
	AHNS_DEBUG("ap_thread::init_state_estimator:: set up A matrix");
	for (ii = 1; ii < 5; ii++)
	{
		(*A)(ii,ii) = 1;
	}
	
	AHNS_DEBUG("ap_thread::init_state_estimator:: set up C matrix");
	(*C)(1,1) = 1;
	(*C)(3,3) = 1;

	AHNS_DEBUG("ap_thread::init_state_estimator:: set up S_z and S_w matrices");
	for (ii = 1; ii < 5; ii++)
	{
		(*S_z)(ii,ii) = 1;
	}
	for (ii = 1; ii < 5; ii++)
	{
		(*S_w)(ii,ii) = 1;
	}
	AHNS_DEBUG("ap_thread::init_state_estimator:: P matrix ( " << *P << " ).");
}

/**
 * Deletes all the memory used by the state estimator
 */
void ap_thread::state_estimator_cleanup()
{
	delete A;
	delete B;
	delete C;
	delete x;
	delete u;
	delete y;
	delete P;
	delete s;
	delete S_z;
	delete S_w;
	delete K;
	delete alt_av;
	delete period_av;
	delete batt_av;
}

/**
 * The controller. Uses PID loops to calculate commansd for the UAV.
 *
 * pre::	valid state data
 *
 * post::	sends corresponding commands to groundpulse
 *		returns integer value indicating success of function
 *
 * @param *heli A pointer to the state structer representing the helicopter
 */
int ap_thread::Controller(State *heli)
{
	AHNS_DEBUG("ap_thread::Controller()");
	heli_command prev_cmd;
	

	prev_cmd = output;

	AHNS_DEBUG("ap_thread::Controller:: heli->roll (" << setbase(16) << heli->roll << ")");
	
	//TODO: this could be better - jsut don't send a command to groundpulse if it is bad. groundpulse should just keep going
	try{
		output.lateral = roll2lat->run(roll_cmd, heli->roll, heli->time, latPID_vals); 
		output.longitudinal = pitch2long->run(pitch_cmd, heli->roll, heli->time, lonPID_vals);
		output.rudder = yaw2rud->run(yaw_cmd, heli->yaw, heli->time, rudPID_vals);
		output.collective = alt2col->run(altitude_cmd, heli->z, heli->time, colPID_vals);
	}
	catch(int n)
	{
		AHNS_DEBUG("ap_thread::Controller:: caught exception from PID: ( " << n << " )");
		output = prev_cmd;
	}

	AHNS_DEBUG("ap_thread::Controller output collective: (" << output.collective << ")");

	transmit(output, *trim_vals, groundpulse_port);
	
	live_cmd = output;
	emit new_cmd();

	return 0;
}

