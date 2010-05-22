/**
 * @file   imuTester.c
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-05-20 01:32:15 +1000 (Thu, 20 May 2010) $
 * $Rev: 151 $
 * $Id: bfcameracomms.cpp 151 2010-05-19 15:32:15Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Driver file to test the IMU serial library and transmit UDP packets with
 * the UDP server
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state.h"
#include "server.h"
#include "commands.h"
#include "udp.h"
#include "imuserial.h"

int verbose = 0;
int server_port	= 2002;

char *server_host;
char *imu_serial_port;
static Server server;
unsigned char init = 1;
struct timeval timestamp,local_time;
char *file;
char text[128];

// IMU timing
double startTime, endTime, diffTime;

int main(int argc, char *argv[])
{
  const char *program = argv[0];
  int c, errflag = 0;

  server_host = (char *) malloc(100*sizeof(char));
  imu_serial_port = (char *) malloc(100*sizeof(char));
  file = (char *) malloc(100*sizeof(char));

  if(server_host == NULL)
  {
    fprintf(stderr, "Cannot allocate mem\n");
    exit(-1);
  }

  // default server host
  server_host = "127.0.0.1";
  // default imu serial port
  imu_serial_port = "/dev/ttyUSB0";

  while((c = getopt(argc, argv, "i:s:p:v")) != -1)
  {
    switch(c)
    {
      case 'p':
        server_port = atoi(optarg);
        fprintf(stderr, "port: %d\n", server_port);
        break;
      case 's':
        server_host = optarg;
        fprintf(stderr, "server: %s\n", server_host);
        break;
      case 'v':
        verbose = atoi(optarg);
        fprintf(stderr, "verbose %d\n", verbose);
        break;
      case 'i':
        imu_serial_port = optarg;
        fprintf(stderr, "IMU serial port: %s\n", imu_serial_port);
        break;
      case '?':
        ++errflag;
        break;
    }
  }
  // connect to the IMU
  if (!openSerial(imu_serial_port, BAUD_RATE_DEFAULT))
  {
    fprintf(stderr,"Cannot connect to IMU\n");
    return -1;
  }
  // save the startTime
  gettimeofday(&timestamp, NULL); 
  startTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);

  msg_camera_t cam_msg;
  // initialise the state
  state_t state;
  int count=0;
  int type;
    
  server_init(&server, 2002); 
  server_handle(&server, CAM_STATE, server_cam_state, (void*) &cam_msg);
  memset((void *)(&cam_msg), 0, sizeof(msg_camera_t));
  memset((void *)(&state), 0, sizeof(state_t));

  while(1)
  {
    //gettimeofday(&local_time,0);
    int type=0;
    if(server_poll(&server,1))
    {
      type = server_get_packet(&server);
    }    
    if(type < 0)
    {
      fprintf(stderr,"Read error from server");
      return -1;
    }
    count++;
    //usleep(20000); // 100hz
    //send data to the socket
    if(count%1 == 0)
    {
      init = 1;
    }

    if(init)
    {
      memset((void *)(&state), 0, sizeof(state_t));
      // get the IMU sensor data
      getImuSensorData(&state.p, &state.q, &state.r, &state.ax, &state.ay, &state.az);
      // calculate the time elapsed since last update
      gettimeofday(&timestamp, NULL); 
      endTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      diffTime = endTime - startTime;
      // calculate the displacement in x,y and z
      state.x = state.x + state.vx*diffTime + state.ax*pow(diffTime,2);
      state.y = state.y + state.vy*diffTime + state.ay*pow(diffTime,2);
      state.z = state.z + state.vz*diffTime + state.az*pow(diffTime,2);
      // calculate the velicty in x,y and z
      state.vx = state.vx + state.ax*diffTime;
      state.vy = state.vy + state.ay*diffTime;
      state.vz = state.vz + state.az*diffTime;
      // calculate the frequency (save in voltage)
      state.voltage = 1/(diffTime);
      // restart the clock
      gettimeofday(&timestamp, NULL); 
      startTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      // output the sent states  
      printf( "\n\nSending state \n"),
        fprintf(stderr,"state : %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
		state.p ,
 		state.q ,
 		state.r ,
		state.ax,
                state.ay, 
                state.az,
		state.vx,
                state.vy, 
                state.vz,
		state.x,
                state.y, 
                state.z,
                state.voltage);
      //gettimeofday(&local_time,0);   
      //fprintf(stderr,"time: %d : %d", local_time.tv_sec,local_time.tv_usec);
      // send the server packet
      server_send_packet(&server, HELI_STATE, &state, sizeof(state_t)); 
      init = 0;
    }   
  }
  return 0;
}

