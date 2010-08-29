/**
 * $Author: liamosullivan $
 * $Date: 2010-06-21 15:25:25 +1000 (Mon, 21 Jun 2010) $
 * $Rev: 193 $
 * $Id: imuServerTester.c 193 2010-06-21 05:25:25Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Flight computer main program
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "main.h"
#include "server.h"
#include "state.h"
#include "imuserial.h"
#include "arduserial.h"
#include "kf.h"

// udp server
static Server server;
// state variable
state_t state;
// sensor data variable
sensor_data_t raw_IMU;
// compass heading
double compass_heading = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// flight computer functions
int sensorInit();
int mcuInit();
// flight computer thread functions
void * sendUDPData(void *pointer);
void * updateIMUdata(void *pointer);
void * updateCompassHeading(void *pointer);

int main(int argc, char *argv[])
{
  // boolean variables to check serial connections
  int sensorReady = 0;
  int mcuReady = 0;
  // init the server
  server_init(&server, SERVER_PORT);
  // initialise the sensors
  sensorReady = sensorInit();
  // initialise the MCU
  mcuReady = mcuInit();
  // initialise the Kalman filter
  attitudeFilterInitialise();
  // all systems operational - begin the flight computer
  if (sensorReady && mcuReady)
  {
    // create the flight computer threads
    pthread_t udpThread;
    pthread_t imuThread;
    pthread_t arduThread;
    // create the server thread
    pthread_create(&udpThread, NULL, sendUDPData, (int*) 1);
    // create the imu thread
    pthread_create(&imuThread, NULL, updateIMUdata, (int*) 3);
    // create the arduino thread
    pthread_create(&arduThread, NULL, updateCompassHeading, (int*) 3);
    // execute the udp server thread
    pthread_join(udpThread,NULL);
    // execute the state thread
    pthread_join(imuThread,NULL);
    // execute the compass thread
    pthread_join(arduThread,NULL);
  }
  return 0;
}

// update the compass heading from the arduino
void * updateCompassHeading(void *pointer)
{
  double previous_compass_heading = 0.0;
  while(1)
  {
    // sleep the thread for updating the compass
    usleep(COMPASS_DELAY*1e3);
    // get the compass heading from the arduino
    pthread_mutex_lock(&mut);
    previous_compass_heading = compass_heading;
    if (!getCompassHeading(&compass_heading))
    {
      compass_heading = previous_compass_heading;
    }
    state.x = compass_heading;
    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

// update IMU data
void * updateIMUdata(void *pointer)
{
  // time stamp
  struct timeval timestamp;
  // time between filter updates
  double startFilterTime, endFilterTime, diffFilterTime;
  // calculate filter start time
  gettimeofday(&timestamp, NULL); 
  startFilterTime = timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
  while(1)
  {
    // sleep the thread for updating the IMU
    usleep(IMU_DELAY*1e3);
    pthread_mutex_lock(&mut);
    // get the IMU sensor data
    //getImuSensorData(&state.p, &state.q, &state.r, &state.ax, &state.ay, &state.az);
    getImuSensorData(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az);
    // calculate the time elapsed since last update
    gettimeofday(&timestamp, NULL); 
    endFilterTime = timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
    diffFilterTime = endFilterTime - startFilterTime;
    // calculate filter start time
    gettimeofday(&timestamp, NULL); 
    startFilterTime = timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
    // perform the attitude filtering using the imu data
    attitudeFilter(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az, &state.phi, &state.theta, &state.psi, compass_heading, diffFilterTime);
    printf(">> kf update : %f\n",1/diffFilterTime);

    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

// initialise the sensors connected to the flight computer
// 1. IMU
// 2. Arduino (compass, camera, ultrasonic)
int sensorInit()
{
  int retSerial = 0;
  int retValue =0;
  // connect to the IMU
  retSerial = openIMUSerial(IMU_SERIAL_PORT, IMU_BAUD_RATE);
  if(retSerial)
  {
    retValue = 1;
    printf(">> IMU connected\n");
  } else
    {
      printf(">> Error: Cannot connect to IMU on serial port: %s\n",IMU_SERIAL_PORT);
    }
  // connect to the arduino
  retSerial = openArduSerial(ARDU_SERIAL_PORT, ARDU_BAUD_RATE);
  if(retSerial)
  {
    retValue = 1;
    printf(">> Arduino connected\n");
  } else
    {
      printf(">> Error: Cannot connect to Arduino on serial port: %s\n", ARDU_SERIAL_PORT);
    }

  return retValue;
}

// initialise connection to the MCU
int mcuInit()
{
  printf(">> MCU connected \n");
  return 1;
}

// send the UDP data packets to clients
void * sendUDPData(void *pointer)
{
  // udp packet type
  int udpType = 0;
  int init = 1;
  int count=0;
  struct timeval timestamp;
  double startServerTime = 0;
  double endServerTime = 0;
  double diffServerTime = 0;
  int dataLength;
  unsigned char buffer[512];
  // initialise startTime
  gettimeofday(&timestamp, NULL); 
  startServerTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);

  while(1)
  {
    int type=0;
    if(server_poll(&server,1))
    {
      udpType = server_get_packet(&server);
    }    
    if(udpType < 0)
    {
      fprintf(stderr,"Read error from server");
      //return -1;
    }
    count++;
    usleep(SERVER_DELAY*1e3);
    //send data to the socket
    if(count%1 == 0)
    {
      init = 1;
    }
    if(init)
    {
      // calculate server delay time
      gettimeofday(&timestamp, NULL); 
      endServerTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      diffServerTime = endServerTime - startServerTime;
      // output the sent states  
      printf(">> state : %f %f %f %f | %f\n",raw_IMU.p,raw_IMU.q,raw_IMU.r,state.psi,(1/diffServerTime));
      // send the state packet
      server_send_packet(&server, HELI_STATE, &state, sizeof(state_t));
      // send the raw sensor data packet
      dataLength = PackSensorData(buffer, &raw_IMU);
      server_send_packet(&server, SENSOR_DATA, buffer, dataLength);
      init = 0;
      // reinitialise startTime
      gettimeofday(&timestamp, NULL); 
      startServerTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
    }   
  }
  return NULL;
}

