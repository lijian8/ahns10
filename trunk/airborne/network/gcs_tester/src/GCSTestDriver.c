/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/query_state2.c,v $
 * $Log: query_state2.c,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
 *
 * Revision 1.1.1.1  2008-05-05 06:49:35  luis
 * initial import
 *
 * Revision 1.2  2006/01/18 15:47:45  cvs
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2005/11/06 10:56:35  cvs
 * initial creeation
 *
 * Revision 1.1.1.1  2005/10/22 16:44:22  cvs
 * initial creation
 *
 * Revision 1.1.1.1  2005/10/15 15:14:06  cvs
 * initial project comments
 *
 * Revision 1.1.1.1  2004/03/03 11:03:06  srik
 * Initial Release 0.1
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "state.h"
#include "server.h"
#include "commands.h"


#include "udp.h"


int verbose = 0;
int server_port = 2002;

char *server_host;
static Server server;
unsigned char init = 1;
struct timeval timestamp, local_time;
char text[128];



int
main (int argc, char *argv[])
{
  const char *program = argv[0];

  int c, errflag = 0;

  server_host = (char *) malloc (100 * sizeof (char));

  if (server_host == NULL)
    {
      fprintf (stderr, "cannot allocate mem\n");
      exit (-1);
    }


  server_host = "127.0.0.1";

  while ((c = getopt (argc, argv, "s:p:v:")) != -1)
    {
      switch (c)
	{
	case 'p':
	  server_port = atoi (optarg);
	  fprintf (stderr, "port %d\n", server_port);
	  break;
	case 's':
	  server_host = optarg;
	  fprintf (stderr, "server %s\n", server_host);
	  break;
	case 'v':
	  verbose = atoi (optarg);
	  fprintf (stderr, "verbose %d\n", verbose);
	  break;
	case '?':
	  ++errflag;
	  break;
	}
    }


  msg_camera_t cam_msg;
  state_t state;
  fc_state_t fc_state;

  int count = 0;
  int type;

  server_init (&server, 2002);
  server_handle (&server, CAM_STATE, server_cam_state, (void *) &cam_msg);
  memset ((void *) (&cam_msg), 0, sizeof (msg_camera_t));
  memset ((void *) (&state), 0, sizeof (state_t));
  memset ((void *) (&fc_state), 0, sizeof (fc_state_t));

  while (1)
    {
      gettimeofday (&local_time, 0);
      int type = 0;
      if (server_poll (&server, 1))
	type = server_get_packet (&server);



      if (type < 0)
	{
	  fprintf (stderr, "Read error from server");
	  return -1;
	}


      count++;
      usleep (50e3);		/*50hz*/


      /* send data to the socket */
      if (count % 1 == 0)
	init = 1;

      if (init)
	{
	  memset ((void *) (&state), 0, sizeof (state_t));
  	  memset ((void *) (&fc_state), 0, sizeof (fc_state_t));

	  state.phi = fmod (count / 180.0 * 5.0 / 4.0, M_PI / 2.0);
	  state.theta = fmod (count / 180.0 * 10 / 7.0, M_PI / 2.0);
	  state.psi = fmod (count / 180.0 * 10.0 / 3.0, 2 * M_PI);
          state.p = fmod((count - 500) * count/M_PI*180.0, M_PI/100);
          state.q = fmod((count - 500) * count/M_PI*180.0, M_PI/100);
          state.r = fmod((count - 500) * count/M_PI*180.0, M_PI/100);
          state.x = fmod((count - 300.0) * count/M_PI, 300);
          state.y = fmod((count - 200.0) * count/M_PI, 200);
          state.z = fmod((count - 100.0) * count,100);
	  state.voltage = (count/100 % 4) + 9;


	  printf ("\n\nSending state \n"),
          fprintf (stderr, "state : %f  %f %f \n", state.phi, state.theta, state.psi);
	  gettimeofday (&local_time, 0);
	  fprintf (stderr, "time: %.4f\n", local_time.tv_sec + local_time.tv_usec/1e6);
	  fprintf (stderr, "count: %d\n", count);
	  server_send_packet (&server, HELI_STATE, &state, sizeof (state_t));

	  fc_state.commandedEngine1 = 1000 + fmod(0 + count*2*M_PI, 1000);
	  fc_state.commandedEngine2 = 1000 + fmod(100 + count*3*M_PI, 1000);
	  fc_state.commandedEngine3 = 1000 + fmod(200 + count*4*M_PI, 1000);
	  fc_state.commandedEngine4 = 1000 + fmod(300 + count*5*M_PI, 1000);

	  fc_state.rclinkActive = ((count % 200) < 100 ) ? 1 : 0;
	  fc_state.fcUptime = count % 6000;
          fc_state.fcCPUusage = 2*count;

	  fprintf(stderr,"fc_size: %d\tUpTime: %d\tUsage: %d\tRC Link: %d",sizeof(fc_state_t),fc_state.fcUptime,fc_state.fcCPUusage,fc_state.rclinkActive);
	  unsigned char buffer[200];
	  int dataLength = PackFCState(buffer,&fc_state);
	  server_send_packet (&server, FC_STATE, &fc_state, dataLength);



	  init = 0;
	}
    }
  return 0;
}
