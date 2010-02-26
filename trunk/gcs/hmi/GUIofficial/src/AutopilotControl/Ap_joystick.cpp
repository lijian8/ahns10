#include <cstdio>
#include <cstdlib>
#include <unistd.h> // header defines miscellaneous symbolic constants and types, and declares miscellaneous functions.
#include <fcntl.h>
#include <cmath>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include "ahns_logger.h"
#include "ApControl.h"
#include "include/GUIofficial.h"

/**
 * Initialisation of the Autopilot control class for joystick functions
 * @param port The serial port to which the receiver is connected.
 */
void APclass::jsInit(jsStore& data)
{
	int version = 0x0001; // 16-bit integer
	char name[128] = "AHNS2009";
	unsigned char axes;
	unsigned char buttons;

	fd = open("/dev/input/js0", O_RDONLY);

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(128), name);
	data.fd = fd;
}

void APclass::jsFunction(jsStore& data)
{
	AHNS_DEBUG("jsTestThread::jsFunction()");
	fflush(stdout);
	struct js_event js;

	AHNS_DEBUG("jsFunction:: assigning event structures.");
	
	//AHNS_DEBUG("jsFunction:: fflush(stdout)");
	
	
	int rc;
	struct timeval tv;
	AHNS_DEBUG("jsFunction:: rc = select(data.fd + 1, &fds, 0, 0, &tv).");
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(data.fd, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	rc = select(data.fd + 1, &fds, 0, 0, &tv);
	if(rc <= 0)	return;
	
	if (read(data.fd, &js, sizeof(struct js_event)) != sizeof(struct js_event))
	{
		printf("ERROR in READ or something");
		AHNS_DEBUG("jsFunction:: ERROR in READ or something.");
	fflush(stdout);
	}

	switch(js.type & ~JS_EVENT_INIT)
	{
		AHNS_DEBUG("jsFunction:: switch(js.type & ~JS_EVENT_INIT");
		fflush(stdout);

		case JS_EVENT_BUTTON:
			data.button[js.number] = js.value;
			AHNS_DEBUG("jsFunction:: case JS_EVENT_BUTTON   " << js.value << "   " << (int)js.number);
			fflush(stdout);
			break;
		case JS_EVENT_AXIS:
			data.axis[js.number] = js.value;
			AHNS_DEBUG("jsFunction:: case JS_EVENT_AXIS   " << js.value << "   " << (int)js.number);
			fflush(stdout);
			break;
	}

	fflush(stdout);
}

/* ******** FUNCTION USED TO BE CALLED BY EXTERNAL PROGRAM TO RETRIEVE JOYSTICK INFORMATION ******** */
/**
 * Obtains and returns the joystick value for X, Y and Z values. This function calls the joystick functions to acquire the joystick position and assigns the values to the %joystickPacket structure.
 * @return %joystickPacket structure that contains joystick values for the X, Y and Z values normalised to 10000.
 */
joystickPacket APclass::get_joystickPosition()
{
	joystickPacket retJoy;
	
	jsFunction(jsHandle);
	retJoy.joy_X = -(((jsHandle.axis[0]*10000)/JOY_MAX_LIM) - 10000);
	retJoy.joy_Y = (jsHandle.axis[1]*10000)/JOY_MAX_LIM;
	retJoy.joy_Z = -(((jsHandle.axis[2]*10000)/JOY_MAX_LIM) - 10000);
	
	return retJoy;
}


void GUIofficial::db_animateAH()
{
	bankRate = -((udp_JS.joy_X/50) - 100);
	pitchRate = (int)ap_dispJoyLong_SA->value();
	altPos = (int)ap_dispJoyAlt_SA->value();
	AH_Core->angRollRate = (float)(((bankRate/50.0f)*30)/50);
	AH_Core->angPitchRate = (float)(((-pitchRate/50.0f)*10)/50);
	AH_Core->vertAltPos = (float)(altPos);
	AH_Core->UpdateRoll();
}
