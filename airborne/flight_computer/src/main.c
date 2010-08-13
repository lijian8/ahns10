/**
 * @file   main.c
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-06-21 15:25:25 +1000 (Mon, 21 Jun 2010) $
 * $Rev: 193 $
 * $Id: imuServerTester.c 193 2010-06-21 05:25:25Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Flight computer main
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "main.h"
#include "server.h"
#include "state.h"

// udp server
static Server server;
// state variable
state_t state;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// flight computer functions
int sensorInit();
int mcuInit();
// flight computer thread functions
void * sendUDPData(void *pointer);
void * updateStateData(void *pointer);

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
  // all systems operational - begin the flight computer
  if (sensorReady && mcuReady)
  {
    // create the flight computer threads
    pthread_t udpThread;
    pthread_t stateThread;
    // create the server thread
    pthread_create(&udpThread, NULL, sendUDPData, (int*) 1);
    // create the state thread
    pthread_create(&stateThread, NULL, updateStateData, (int*) 2);
    // execute the udp server thread
    pthread_join(udpThread,NULL);
    // execute the state thread
    pthread_join(stateThread,NULL);
  }
  return 0;
}

// update state data
void * updateStateData(void *pointer)
{
  int factor = 1;
  while(1)
  {
    usleep(0.5e6);
    pthread_mutex_lock(&mut);
    state.p = factor+2;
    state.q = factor+5;
    state.r = factor+3;
    factor++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

// initialise the sensors connected to the flight computer
// 1. IMU
// 2. Arduino (compass, camera, ultrasonic)
int sensorInit()
{
  printf(">> IMU connected\n");
  printf(">> Arduino connected\n");
  return 1;
}

// initialise connection to the MCU
int mcuInit()
{
  printf(">> MCU connected\n");
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
  double startTime = 0;
  double endTime = 0;
  double diffTime = 0;
  // initialise startTime
  gettimeofday(&timestamp, NULL); 
  startTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);

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
      pthread_mutex_lock(&mut);
      // wait for the states to be ready
      pthread_cond_wait(&cond, &mut);
      // calculate server delay time
      gettimeofday(&timestamp, NULL); 
      endTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      diffTime = endTime - startTime;
      // output the sent states  
      printf(">> state : %f %f %f | %f\n",state.p,state.q,state.r,diffTime);
      // send the server packet
      server_send_packet(&server, HELI_STATE, &state, sizeof(state_t));
      init = 0;
      // reinitialise startTime
      gettimeofday(&timestamp, NULL); 
      startTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      pthread_mutex_unlock(&mut);
    }   
  }
  return NULL;
}

