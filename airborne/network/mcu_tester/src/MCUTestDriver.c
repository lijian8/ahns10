/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/query_state2.c,v $
 * $Log: query_state2.c,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
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

#include "mcuserial.h"
#include "MCUCommands.h"

#define DEFAULT_SERVER_IP 127.0.0.1

/** Testing Functions */
void FilteredStateData(int count, state_t* testState);
void FCStateData(int count, fc_state_t* testState);
void GainsData(int count, gains_t* testState);
void LoopParametersData(int count, loop_parameters_t* testState);
void SensorData(int count, sensor_data_t* sensorData);

int verbose = 0;
int server_port = 2002;
char *server_host;
static Server server;
unsigned char init = 1;
struct timeval timestamp, local_time;

char* mcu_serial_port;
uint8_t flightMode = 0;


int
main (int argc, char *argv[])
{
  const char *program = argv[0];
  int c, errflag = 0;
  int count = 0;
  int type = 0;
  unsigned char buffer[512];
  int dataLength = 0;

  // Open MCU
  mcu_serial_port= (char*) malloc(100*sizeof(char));
  mcu_serial_port = "/dev/ttyS0";

  if (!mcuOpenSerial(mcu_serial_port,MCU_BAUD_RATE)) 
  {
    fprintf(stderr,"Error - MCU Unable to Open Port\n");
    return -1;
  } 

  // Allocate the Testing Structures
  state_t state;
  fc_state_t fcState;
  ap_state_t apState;
  gains_t gains;
  loop_parameters_t loopParameters;
  sensor_data_t sensorData;

  // Allocate Server
  server_host = (char *) malloc (100 * sizeof (char));
  if (server_host == NULL)
  {
    fprintf (stderr, "ERROR: Cannot allocate mem\n");
    exit (-1);
  }

  server_host = "DEFAULT_SERVER_IP";

  // Parse Commandline Arguments
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

  server_init (&server, server_port);

  while (1)
    {
      gettimeofday (&local_time, 0);
      if (server_poll (&server, 1))
      {
	type = server_get_packet (&server);
      }
      if (type < 0)
	{
	  fprintf (stderr, "Read error from server");
	  return -1;
	}


      count++;
      usleep (20e3);

      if (count % 1 == 0) // non-zero count
      {
	init = 1;
      }

      if (init)
      {
          // Test the Server sending packets
	  gettimeofday (&local_time, 0);
	  
	  // State Test Structure
	  FilteredStateData(count, &state);
	  server_send_packet (&server, HELI_STATE, &state, sizeof (state_t));
  
 	  // FC Test Structure
          FCStateData(count,&fcState);
	  dataLength = PackFCState(buffer,&fcState);
	  server_send_packet (&server, FC_STATE, buffer, dataLength);

	   
          // Sensor Data
          SensorData(count,&sensorData);
          dataLength = PackSensorData(buffer, &sensorData);
          server_send_packet(&server, SENSOR_DATA, buffer, dataLength);
	  init = 0;
	}
    }
  mcuCloseSerial();
  return 0;
}

void FilteredStateData(int count, state_t* testState)
{
  memset ((void *) (testState), 0, sizeof (state_t));
  
  testState->phi = 0;
  testState->theta = 0;
  testState->psi = 0;

  testState->p = 0;
  testState->q = 0;
  testState->r = 0;

  testState->x = 0;
  testState->y = 0;
  testState->z = 0;

  testState->vx = 0;
  testState->vy = 0;
  testState->vz = 0;

  testState->ax = 0;
  testState->ay = 0;
  testState->az = 0;

  testState->trace = 0;
  testState->voltage = 0;

  return;
}

void FCStateData(int count, fc_state_t* testState)
{
  memset ((void *) (testState), 0, sizeof (fc_state_t));
  
  uint16_t commandedEngine[4];

  getMCUPeriodic(&flightMode,commandedEngine);
  
  testState->commandedEngine1 = commandedEngine[0];
  testState->commandedEngine2 = commandedEngine[1];
  testState->commandedEngine3 = commandedEngine[2];
  testState->commandedEngine4 = commandedEngine[3];
  if (flightMode == FAIL_SAFE)
  {
    testState->rclinkActive = 0;
  }
  else
  {
    testState->rclinkActive = 1;
  }
  testState->fcUptime = count;
  testState->fcCPUusage = 0; 

  return;
}

void SensorData(int count, sensor_data_t* sensorData)
{
  sensorData->p = 0;
  sensorData->q = 0;
  sensorData->r = 0;

  sensorData->ax = 0;
  sensorData->ay = 0;
  sensorData->az = 0;

  return;
}
