/* $Author: luis $
 * $Name:  $
 * $Id: joystick.c,v 1.1.1.1 2008-05-05 07:07:05 luis Exp $
 * $Source: /home/luis/cvsroot/aerial-simulator/joystick/joystick.c,v $
 * $Log: joystick.c,v $
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // header defines miscellaneous symbolic constants and types, and declares miscellaneous functions.
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "joystick.h"

static int (*event)(int fd, struct js_event *e, int usec);
static int reset = 0;
static const char *prog = "heli_joy";
static const char *joy_dev = "/dev/input/js0";
static const double pi = M_PI;


int joydev_open(const char *name)
/*	PRE:  name of file
	POST: file descriptor (fd)
	PROC: 
*/
{
	int fd;
	int value = 0;

	fd = open(name, O_RDONLY); /* Opening file 'name' for reading only (O_RDONLY) = return file descriptor */
	/* File Descriptor (fd): Integer associated with file. Enables rd/wr from file using integer instead filename. */
	if(fd < 0){
		perror(name); /* 'perror' prints an error message to stderr, based on the error state stored in errno. */
		return -1;
	} /* File Descriptor (fd) should not return a -ve number, if it does, error occured */

	/* Perform the I/O control operation specified by REQUEST on FD.
   	One argument may follow; its presence and type depend on REQUEST.
   	Return value depends on REQUEST.  Usually -1 indicates error. */
	if(ioctl(fd, JSIOCGAXES, &value) == 0){
		fprintf(stderr, " Joystick has %d axis\n", value);
	} /* For AXES */
	if(ioctl(fd, JSIOCGBUTTONS, &value) == 0){
		fprintf(stderr, " Joystick has %d buttons\n", value);
	} /* For BUTTONS */
	/*
	A Unix ioctl call takes as parameters:
	   1. an open file descriptor
	   2. a request code number
	   3. either int val, possibly unsigned (going to the driver) or a ptr to data (either go to or come back from driver).
	****************************************
	int fprintf(FILE *stream, const char *format, ...)
	fprintf enables printf output to be written to any file.
	Programmers frequently use it to print errors, by writing to the standard error (stderr) device,
	but it can operate with any file opened with the fopen function.
	*/

	return fd;
}

int joydev_close(int fd)
{
	close(fd);
	return 0;
}

int joydev_event(int fd, struct js_event *e, int usecs)
{
	int rc;
	struct timeval tv;
	/*
	struct timeval { 
	    long    tv_sec;         // seconds
	    long    tv_usec;        // microseconds
	};
	*/

	fd_set fds; /* Defining Structure 'fd_set' */

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = usecs;

	rc = select(fd + 1, &fds, 0, 0, usecs >= 0 ? &tv:0);
	/*
	**** usecs >= 0 ? &tv:0 ****
	If usecs >= 0
		return &tv;
	else
		return 0
	*/

	if(rc <= 0)
		return rc;
	return read(fd, e, sizeof(*e)); /* Read obj 'e' from 'fd', given the size of '*e' */
}

// ADDED LINES

// static void handle_axis(int axis, int value)
static double handle_axis(int axis, int value)
{
	//joy_axis_t *values;
	//trim_t	*trim;
	double scaled = value;

	if(axis < 0 || MAX_AXES < axis){
	    fprintf(stderr, "%s invalid axis %d\n", prog, axis);
	    return 0;
	}
	
	//values = &limits[axis];
	//trim = &trims[axis];

	scaled *= (20400/255);
	scaled -= (128)*(20400/255);
	double debugVal = scaled;

	fprintf(stderr, "axis %d value %f\n", axis, debugVal);

	//convert to radians
	scaled *= pi/180.0; 

	//values->last = scaled;

	return debugVal; // LINE ADDED
}

static void handle_button(int button)
{
	if(button == 0){
		reset = 1;
		return;
		
	}
//	fprintf(stderr, "%s unhandled button %d\n", prog, button);
      
}

double handle_joystick(int fd)
{
	struct js_event e;
	int rc;
	double js_buffer;

	rc = event(fd, &e, -1);
	if(rc < 0){
	    fprintf(stderr, "%s joystick event failed\n", prog);
	    exit(-1);
	}
	
	if(rc == 0) return 0;
	

	fprintf(stderr, "%d %d axis \n", e.type, JS_EVENT_AXIS);

	if(e.type & JS_EVENT_AXIS)//{
	  //handle_axis(e.number, e.value);//fprintf(stderr, "one \n");}
	  js_buffer = handle_axis(e.number, e.value);//fprintf(stderr, "one \n");}
	else if(e.type & JS_EVENT_BUTTON)//{
	  handle_button(e.number);//fprintf(stderr, "two \n");}
	else
	    fprintf(stderr, "%s unknown event type %02x\n", prog, e.type);

	return js_buffer;
}

/*
static void handle_state(int fd)
{
  //state_t state;
	int axis;
	for(axis = 0; axis < 9; axis++)
	{
		//joy_axis_t *values = &limits[axis];
		if(!values->name || !values->name[0]) continue;
	}
}
*/

int initJoystick(void)
{
	int joy_fd;

	joy_fd = joydev_open(joy_dev);
	event = joydev_event;

	if(joy_fd < 0){
		fprintf(stderr, "%s unable to open joystick %s\n", prog, joy_dev);
		return 0;
	}

	return joy_fd;
}
