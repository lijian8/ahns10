#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include "../txUDP.h"
#include "../../../../reuse/ahns_logger.h"

#include <linux/joystick.h>

void jsTestThread::jsInit(jsStore& data)
{
	//ahns_logger_init("jsTestThread.log");

	int fd;
	int version = 0x000800;
	char name[128] = "Unknown";
	unsigned char axes;
	unsigned char buttons;


	fd = open("/dev/input/js0", O_RDONLY);

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(128), name);
	data.fd = fd;

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");
}

void jsTestThread::jsFunction(jsStore& data)
{
	AHNS_DEBUG("jsTestThread::jsFunction()");
	fflush(stdout);
	struct js_event js;

	AHNS_DEBUG("jsFunction:: assigning event structures.");
	;
	AHNS_DEBUG("jsFunction:: fflush(stdout)");
	
	
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
			std::cerr << "js.number button   " << (int)js.value << std::endl;
			AHNS_DEBUG("jsFunction:: case JS_EVENT_BUTTON   " << js.value << "   " << (int)js.number);
			fflush(stdout);
			break;
		case JS_EVENT_AXIS:
			data.axis[js.number] = js.value;
			std::cerr << "js.number axis   " << (int)js.value << std::endl;
			AHNS_DEBUG("jsFunction:: case JS_EVENT_AXIS   " << js.value << "   " << (int)js.number);
			fflush(stdout);
			break;
	}

	fflush(stdout);
}
