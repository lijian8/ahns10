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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "joystick.h"

int joydev_open(const char *name)
{
	int fd;
	int value = 0;

	fd = open(name, O_RDONLY);
	if(fd < 0){
		perror(name);
		return -1;
	}

	if(ioctl(fd, JSIOCGAXES, &value) == 0){
		fprintf(stderr, " Joystick has %d axis\n", value);
	}
	if(ioctl(fd, JSIOCGBUTTONS, &value) == 0){
		fprintf(stderr, " Joystick has %d buttons\n", value);
	}

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
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	rc = select(fd + 1, &fds, 0, 0, &tv);
	if(rc <= 0)
		return rc;
	return read(fd, e, sizeof(*e));
}
