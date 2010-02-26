/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-10-06 14:23:39 +1000 (Tue, 06 Oct 2009) $
 * $Rev: 759 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Code for transmitting control surface commands from the autopilot computer to groundpulse. 
 */

#ifndef AHNS_UPLINK
#define AHNS_UPLINK

namespace uplink{

/**
 * This structure represents a set of commands for the helicopter. Four control channels.
 */
struct heli_command{
	double lateral;
	double longitudinal;
	double collective;
	double rudder;
};

/**
 * This structure represents the commands for the helicopter. It is a less abstract, lower level version of %heli_command where each value in the double array represents a channel for the transmitter.
 */
struct heli_command_db{
	double channel[7];
};

/**
 * The trim values for each channel.  The actual trimming is implemented in the %transmit function.
 */
struct heli_trims{
	double roll;
	double pitch;
	double rudder;
	double collective;
};

/**
 * Function which sends the commands to groundpulse.
 */
int transmit(heli_command cmd, heli_trims trim_vals, int serial_port);

/**
 * Function which sends commands to the helicopter.  It takes a %heli_command_db as opposed to a %heli_command and is only used for debugging.
 */
int transmit_db(heli_command_db cmd, int serial_port);

}

#endif
	
