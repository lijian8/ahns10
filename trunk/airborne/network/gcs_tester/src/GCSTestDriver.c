/* $Author$
 * $Name:  $
 * $Id$
 * $Source: /home/luis/cvsroot/net-api/query_state2.c,v $
 * $Log: query_state2.c,v $
 * Revision 1.1.1.1  2008-05-05 07:07:57  luis
 * initial import
 *
 * @brief Test Program for the Development of the GCS
 *        Currently Transmits:
 *         - state_t
 *         - fc_state_t
 *         - COMMAND_ACK
 *        
 *        Currently Receives:
 *         - COMMAND_CLOSE
 *         - COMMAND_OPEN
 *
 *
 * @TODO
 *        - Gains
 *        - Loop Parameters
 *        - AP Get, Set and Save CONFIG
 *        - IMU Structure
 *        - Raw Sensor Data
 *        - Desired Position
 *        - Desired Attitude
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "state.h"
#include "server.h"
#include "commands.h"

#include "udp.h"
#include "control.h"

#define DEFAULT_SERVER_IP 127.0.0.1

/** Testing Functions */
void FilteredStateData(int count, state_t* testState);
void FCStateData(int count, fc_state_t* testState);
void APStateData(int count, ap_state_t* testState);
void GainsData(int count, gains_t* testState);
void LoopParametersData(int count, loop_parameters_t* testState);
void SensorData(int count, sensor_data_t* sensorData);

int verbose = 0;
int receive = 0;
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
  int count = 0;
  int type = 0;
  unsigned char buffer[512];
  int dataLength = 0;

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
  while ((c = getopt (argc, argv, "s:p:v:r:")) != -1)
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
        case 'r':
          receive = atoi(optarg);
          fprintf (stderr, "receiver testing %d\n", receive);
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

      if (init && (receive != 1))
      {
          // Test the Server sending packets
	  gettimeofday (&local_time, 0);
          fprintf (stderr, "Time: %.4f\n", local_time.tv_sec + local_time.tv_usec/1e6);
	  fprintf (stderr, "Count: %d\n", count);
	  
	  // State Test Structure
	  FilteredStateData(count, &state);
	  printf ("\n\nSending state: Size %d \n",sizeof(state_t)),
          fprintf (stderr, "phi = %f   \t theta = %f \t psi = %f \n", state.phi, state.theta, state.psi);
          fprintf (stderr, "p = %f     \t q = %f     \t r = %f \n", state.p, state.q, state.r);
          fprintf (stderr, "x = %f     \t y = %f     \t z = %f \n", state.x, state.y, state.z);
          fprintf (stderr, "vx = %f    \t vy = %f    \t vz = %f \n", state.vx, state.vy, state.vz);
          fprintf (stderr, "ax = %f    \t ay = %f    \t az = %f \n", state.ax, state.ay, state.az);
          fprintf (stderr, "trace = %f \t voltage = %f", state.trace, state.voltage); 
	  server_send_packet (&server, HELI_STATE, &state, sizeof (state_t));
  
 	  // FC Test Structure
          FCStateData(count,&fcState);
	  printf ("\n\nSending fcState: Size %d \n",sizeof(fc_state_t)),
          fprintf (stderr, "Eng1 = %d \t Eng2 = %d \t Eng3 = %d \t Eng4 = %d \n", fcState.commandedEngine1, fcState.commandedEngine2, fcState.commandedEngine3, fcState.commandedEngine4);
          fprintf (stderr, "fcUptime = %llu \t fcCPUusage = %d \t rclinkActive = %d \n", fcState.fcUptime, fcState.fcCPUusage, fcState.rclinkActive);
	  dataLength = PackFCState(buffer,&fcState);
	  server_send_packet (&server, FC_STATE, buffer, dataLength);

          // AP Test Structure
          APStateData(count, &apState);
          printf ("\n\nSending apState: Size %d \n",sizeof(ap_state_t)),
          fprintf (stderr, "referencePhi = %f   \t referenceTheta = %f \t referencePsi = %f \n", apState.referencePhi, apState.referenceTheta, apState.referencePsi);
          fprintf (stderr, "referenceX = %f \t referenceY = %f \t referenceZ = %f \n", apState.referenceX, apState.referenceY, apState.referenceZ);
          fprintf (stderr, "phiActive = %u \t thetaActive = %u \t psiActive = %u \n", apState.phiActive, apState.thetaActive, apState.psiActive);
          fprintf (stderr, "xActive = %u \t yActive = %u \t zActive = %u \n", apState.xActive, apState.yActive, apState.zActive);

	  dataLength = PackAPState(buffer,&apState);	  
          server_send_packet (&server, AUTOPILOT_STATE, buffer, dataLength);
	   
          // Sensor Data
          SensorData(count,&sensorData);
          printf("\n\nSending Sensor Data: Size %d \n",sizeof(sensor_data_t));
          dataLength = PackSensorData(buffer, &sensorData);
          server_send_packet(&server, SENSOR_DATA, buffer, dataLength);
	  init = 0;
	}
        else if (receive == 1)
        {
          // Test the Server's Response to Packets
          APStateData(count, &apState);
          printf ("\n\nSending apState: Size %d \n",sizeof(ap_state_t)),
          fprintf (stderr, "referencePhi = %f   \t referenceTheta = %f \t referencePsi = %f \n", apState.referencePhi, apState.referenceTheta, apState.referencePsi);
          fprintf (stderr, "referenceX = %f \t referenceY = %f \t referenceZ = %f \n", apState.referenceX, apState.referenceY, apState.referenceZ);
          fprintf (stderr, "phiActive = %u \t thetaActive = %u \t psiActive = %u \n", apState.phiActive, apState.thetaActive, apState.psiActive);
          fprintf (stderr, "xActive = %u \t yActive = %u \t zActive = %u \n", apState.xActive, apState.yActive, apState.zActive);

	  dataLength = PackAPState(buffer,&apState);	  
          server_send_packet (&server, AUTOPILOT_STATE, buffer, dataLength);
          init = 0;
        }
    }
  return 0;
}

void FilteredStateData(int count, state_t* testState)
{
  memset ((void *) (testState), 0, sizeof (state_t));
  
  testState->phi = fmod (count / 180.0 * 5.0 / 4.0, M_PI / 2.0);
  testState->theta = fmod (count / 180.0 * 10 / 7.0, M_PI / 2.0);
  testState->psi = fmod (count / 180.0 * 10.0 / 3.0, 2 * M_PI);

  testState->p = fmod((count - 500) * count/M_PI*180.0, M_PI/100);
  testState->q = fmod((count - 500) * count/M_PI*180.0, M_PI/100);
  testState->r = fmod((count - 500) * count/M_PI*180.0, M_PI/100);

  testState->x = fmod((count - 300.0) * count/M_PI, 300);
  testState->y = fmod((count - 200.0) * count/M_PI, 200);
  testState->z = fmod((count - 100.0) * count,100);

  testState->vx = fmod((count - 300.0) * count/M_PI, 50);
  testState->vy = fmod((count - 200.0) * count/M_PI, 50);
  testState->vz = fmod((count - 100.0) * count,100);

  testState->ax = fmod((count - 300.0) * count/M_PI, 2*9);
  testState->ay = fmod((count - 200.0) * count/M_PI, 5*9);
  testState->az = fmod((count - 100.0) * count,3*9);

  testState->trace = count % 50;
  testState->voltage = (count/100 % 4) + 9;

  return;
}

void FCStateData(int count, fc_state_t* testState)
{
  memset ((void *) (testState), 0, sizeof (fc_state_t));
  
  testState->commandedEngine1 = 1000 + fmod(0 + count*2*M_PI, 1000);
  testState->commandedEngine2 = 1000 + fmod(100 + count*3*M_PI, 1000);
  testState->commandedEngine3 = 1000 + fmod(200 + count*4*M_PI, 1000);
  testState->commandedEngine4 = 1000 + fmod(300 + count*5*M_PI, 1000);
  testState->rclinkActive = ((count % 200) < 100 ) ? 1 : 0;
  testState->fcUptime = count++;
  testState->fcCPUusage = 2*count % 100;

  return;
}

void SensorData(int count, sensor_data_t* sensorData)
{
  sensorData->p = fmod(count,10);
  sensorData->q = 2 + fmod(count,10);
  sensorData->r = 3 + fmod(count,10);

  sensorData->ax = fmod(count,100);
  sensorData->ay = fmod(count,50);
  sensorData->az = fmod(count,5);

  return;
}

void APStateData(int count, ap_state_t* testState)
{
  MutexLockAllLoops();
  
  testState->referencePhi = rollLoop.reference;
  testState->referenceTheta = pitchLoop.reference;
  testState->referencePsi = yawLoop.reference;

  testState->referenceX = xLoop.reference;
  testState->referenceY = yLoop.reference;
  testState->referenceZ = zLoop.reference; 

  testState->phiActive = rollLoop.active;
  testState->thetaActive = pitchLoop.active;
  testState->psiActive = yawLoop.active;

  
  testState->xActive = xLoop.active;
  testState->yActive = yLoop.active;
  testState->zActive = zLoop.active;
  
  MutexUnlockAllLoops();

  return;
}
