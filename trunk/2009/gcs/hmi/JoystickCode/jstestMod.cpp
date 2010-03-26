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

#include <linux/joystick.h>

struct jsStruct
{
	int axs[2];
	int tbn[2];
};

struct jsStore
{
	int fd;
	int *axis;
	int *button;
	unsigned char axes;
	unsigned char buttons;
};

struct jsStore jsInit()
{
	int fd;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[128] = "Unknown";
	struct jsStore jsReturn;

	fd = open("/dev/input/js0", O_RDONLY);

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(128), name);
	jsReturn.fd = fd;
	jsReturn.axes = axes;
	jsReturn.buttons = buttons;

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");

//	int *axis;
//	int *button;
//	int i;
//	struct js_event js;

	jsReturn.axis = (int*)calloc(axes, sizeof(int));
	jsReturn.button = (int*)calloc(buttons, sizeof(char));

	return jsReturn;
}

struct jsStruct jsFunction(int fd, int *axis, int *button, unsigned char axes, unsigned char buttons)
{
	struct js_event js;
	struct jsStruct jsRetVal;

	read(fd, &js, sizeof(struct js_event));

	switch(js.type & ~JS_EVENT_INIT) {
	case JS_EVENT_BUTTON:
		button[js.number] = js.value;
		break;
	case JS_EVENT_AXIS:
		axis[js.number] = js.value;
		break;
	}

	if (axes) {
//		printf("Axes: ");
//		for (int i = 0; i < 1/*axes*/; i++)
//		{
			jsRetVal.axs[0] = axis[0];
			jsRetVal.axs[1] = axis[1]; // Axis is an integer array, holds all axis's information
//		}
	}

	if (buttons) {
//		printf("Buttons: ");
//		for (int i = 0; i < 1/*buttons*/; i++)
//		{
			//printf("%2d:%s ", 1, button[1] ? "on " : "off");
			//std::cout << button[1];
			jsRetVal.tbn[0] = button[0];
			jsRetVal.tbn[1] = button[1];
//		}
	}
	fflush(stdout);

	return jsRetVal;
}

int main(int argc, char* argv[])
{
	struct jsStore jsHandle;
	struct jsStruct jsRetVal;

	jsHandle = jsInit();

	while (1)
	{
		jsRetVal = jsFunction(jsHandle.fd, jsHandle.axis, jsHandle.button, jsHandle.axes, jsHandle.buttons);
//	printf("\r");
		printf("\r%5d  %5d  %5d  %5d", jsRetVal.axs[0], jsRetVal.axs[1], jsRetVal.tbn[0], jsRetVal.tbn[1]);
		//std::cout << std::showpos << "Joystick value: " << jsRetVal.axs << "   Button value: " << jsRetVal.tbn;
	}

	return 0;
}
