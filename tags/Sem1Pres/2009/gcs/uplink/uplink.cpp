/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-10-17 16:00:35 +1000 (Sat, 17 Oct 2009) $
 * $Rev: 853 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Code for transmitting control surface commands from the autopilot computer to groundpulse via a serial port.
 */

#include "uplink.h"
#include <stdint.h>
#include <iostream>
#include <iomanip>

#include "ahns_logger.h"

#define CH_OFFSET 127;

/**
 * The uplink namespace.
 */
namespace uplink{

/**
 * Takes commands to be sent to heli, and sends them.
 * @param cmd The commands for the helicopter.
 * @param serial_port The serial port to which groundpulse is connected.
 * @return The return value of the serial write function.
 */
int transmit(heli_command cmd, heli_trims trim_vals, int serial_port)
{
	double lateral = cmd.lateral + trim_vals.roll;
	double longitudinal = cmd.longitudinal + trim_vals.pitch;
	double collective = cmd.collective + trim_vals.collective;
	double rudder = cmd.rudder + trim_vals.rudder;

	AHNS_DEBUG("uplink~transmit()");
	AHNS_DEBUG("transmit:: collective ( " << collective << " )");

	uint8_t channel[11];
	channel[0] = 0x23;
	channel[1] = 0x23;
	channel[2] = 0x23;
	channel[3] = 0x23;
	channel[4] = collective + CH_OFFSET;
	channel[5] = lateral + CH_OFFSET;
	channel[6] = longitudinal + CH_OFFSET;
	channel[7] = rudder + CH_OFFSET;
	channel[8] = 0 + CH_OFFSET;
	channel[9] = 0 + CH_OFFSET;
	channel[10] = 0 + CH_OFFSET;

	return write(serial_port,channel,sizeof(channel));
}

/**
 * Takes commands to be sent to heli, and sends them.
 * @param cmd The commands for the helicopter.
 * @param serial_port The serial port to which groundpulse is connected.
 * @return The return value of the serial write function.
 */
int transmit_db(heli_command_db cmd, int serial_port)
{
	uint8_t channel[11];
	int ii;

	for (ii=0;ii<4;ii++)
	{
		channel[ii] = 0x23;
	}
	for (ii=0;ii<7;ii++)
	{
		channel[ii+4] = cmd.channel[ii] + CH_OFFSET;
	}

	return write(serial_port,channel,sizeof(channel));
}

}


