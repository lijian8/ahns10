/* $Author: luis $
 * $Name:  $
 * $Id: heli_joy.c,v 1.2 2008-05-06 03:44:01 luis Exp $
 * $Source: /home/luis/cvsroot/aerial-simulator/joystick/heli_joy.c,v $
 * $Log: heli_joy.c,v $
 * Revision 1.2  2008-05-06 03:44:01  luis
 * *** empty log message ***
 *
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
#include <math.h>

#include "joystick.h"


static int (*event)(int fd, struct js_event *e, int usec);
static int reset = 0;
static const char *prog = "heli_joy";
static const char *joy_dev = "/dev/input/js0";
static const double pi = M_PI;


static void handle_axis(int axis, int value)
{
	joy_axis_t *values;
	trim_t	*trim;
	double scaled = value;

	if(axis < 0 || MAX_AXES < axis){
	    fprintf(stderr, "%s invalid axis %d\n", prog, axis);
	    return;
	}
	
	values = &limits[axis];
	trim = &trims[axis];

	if(!values->name || !values->name[0]){
	  // fprintf(stderr, "%s unhandled axis %d\n", prog, axis);
	  return;
	}
	//convert the reading into -1.0 to 1.0
	/* scaled *= 10000/255;
	scaled -= 128 * (10000/255); */
	scaled -= values->min;
	scaled /= (values->max - values->min);

	//convert them to degrees
	scaled *= trim->max_deflection - trim->min_deflection;
	scaled += trim->min_deflection;
	scaled += trim->trim;
	scaled=scaled/50.0;

	fprintf(stderr, "axis %d value %f\n", axis, scaled);
	//covert to radians
	scaled *= pi/180.0;

	values->last = scaled;
}

static void handle_button(int button, int value)
{
	if(button == 0){
		/*
		if(value == 1){
			fprintf(stderr, "reset\n");
			reset = 1;
			return;
		}
		*/
	  //fprintf(stderr, "%s Sent reset, button %d\n", prog, button);
		reset = 1;
		return;
		
	}
//	fprintf(stderr, "%s unhandled button %d\n", prog, button);
      
}

static void handle_joystick(int fd)
{
	struct js_event e;
	int rc;

	rc = event(fd, &e, 0);

	if(rc < 0){
	    fprintf(stderr, "%s joystick event failed\n", prog);
	    exit(-1);
	}
	
	//if(rc == 0)
	    //return;
	

	fprintf(stderr, "%d %d axis \n", e.type, JS_EVENT_AXIS);

	if(e.type & JS_EVENT_AXIS)//{
	  handle_axis(e.number, e.value);//fprintf(stderr, "one \n");}
	else if(e.type & JS_EVENT_BUTTON)//{
	  handle_button(e.number, e.value);//fprintf(stderr, "two \n");}
	else
	    fprintf(stderr, "%s unknown event type %02x\n", prog, e.type);
}

static void handle_state(int fd)
{
  //state_t state;
	int axis;
	int rc;

	/* 	//rc = server_get_packet(&server); */
	/* 	if(rc < 0){ */
	/* 	  fprintf(stderr, "%s state read failed\n", prog); */
	/* 	  exit(-1); */
	/* 	} */
	
	/* 	if(rc == 0) */
	/* 	  return */;
	for(axis = 0; axis < 9; axis++){
	  joy_axis_t *values = &limits[axis];
	  
	  //fprintf(stderr, "%d axis %02x\n", axis, values->last);
	  
	  if(!values->name || !values->name[0])
	    continue;
	  //set_parameter(values->name, values->last);
	  
	  //server_send_parameter(&server, values->name, values->la if (axis = 0)
	  // server_send_parameter(&server,SERVO_ROLL, values->last);
	  //server_send_parameter(&server, 20 + axis, values->last);
	  
	  //if (axis == 0)
	  //server_send_parameter(&server,SERVO_ROLL, values->last);
	  
	  //if (axis == 1)
	  //server_send_parameter(&server,SERVO_PITCH, values->last);
	}
/* 	if(reset == 1){ */
/* 	  server_send_parameter(&server, SIM_RESET, reset); */
/* 	  reset = 0; */
/* 	} */
}

int main(int argc, char *argv[])
{
	int joy_fd;
	int server_fd;

	joy_fd = joydev_open(joy_dev);
	event = joydev_event;

	if(joy_fd < 0){
		fprintf(stderr, "%s unable  to open joystick %s\n", prog, joy_dev);
		return EXIT_FAILURE;
	}

	//server_init(&server, 0);
	//server_connect(&server, "127.0.0.1", 2002);
       
	server_fd = 0;
	//fprintf(stderr, " server_fd %d\n", server_fd);
	//if(server_fd < 0)
	//exit(-1);

	while(1){
		int rc;
		fd_set fds;
		int max = joy_fd > server_fd ? joy_fd:server_fd;

		//FD_ZERO(&fds);
		//FD_SET(joy_fd, &fds);
		//FD_SET(server_fd, &fds);

		//rc = select(max +1, &fds, 0, 0, 0);

		//if(rc < 0){
		//	perror("select");
		//	exit(-1);
		//} 
		////if(rc == 0){
		//	continue;
		//}
		        //if(FD_ISSET(joy_fd, &fds))
			handle_joystick(joy_fd);
		//	if(FD_ISSET(server_fd, &fds))
		//handle_state(server_fd);
	}
	return 0;
}
	
