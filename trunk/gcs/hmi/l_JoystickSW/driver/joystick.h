/* $Author: luis $
 * $Name:  $
 * $Id: joystick.h,v 1.1.1.1 2008-05-05 07:07:05 luis Exp $
 * $Source: /home/luis/cvsroot/aerial-simulator/joystick/joystick.h,v $
 * $Log: joystick.h,v $
 * Revision 1.1.1.1  2008-05-05 07:07:05  luis
 * initial import
 *
 * Revision 1.1.1.1  2008-05-05 06:45:08  luis
 * initial import
 *
 * Revision 1.1.1.1  2004/03/03 11:03:06  srik
 * Initial Release 0.1
 *
 */
#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <linux/joystick.h>

#define MAX_AXES	8
#define MAX_BUTTONS	6
typedef struct {
	const char * name;
	int min;
	int max;
	double last;
}joy_axis_t;

typedef struct {
	double min_deflection;
	double max_deflection;
	double trim;
}trim_t;

int joydev_open(const char *name);
int joydev_event(int fd, struct js_event *e, int usec);
int joydev_close(int fd);

static joy_axis_t limits[8] = {
	{"roll", -20269, 23647, 0},
	{"pitch", -18269, 20647, 0},
	{"coll", -18242, 20944, 0},
	{"", 0, 0, 0},
	{"yaw", -23269, 21947, 0},
};

static trim_t trims[MAX_AXES] = {
	{-18.00, 18.00, 0.58},
	{15.00, -15.00, -0.18},
	{10.80, 6.50, 0.00},
	{0, 0, 0},
	{-90.00, 90.00, 6.00},
};
#endif
