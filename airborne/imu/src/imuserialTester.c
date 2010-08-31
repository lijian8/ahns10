/**
 * @file   imuserialTester.c
 * @author Liam O'Sullivan
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * IMU serial library tester
 *
 */
#include <stdlib.h>
#include "imuserial.h"
#include "state.h"

char* imu_serial_port;
unsigned char reg = 'C';
state_t state;

int main(int argc, char* argv[])
{
  // initialise the imu_serial_port array
  imu_serial_port = (char *) malloc(100*sizeof(char));
  // allocate state memory
  memset((void *)(&state), 0, sizeof(state_t));
  // default imu serial port
  imu_serial_port = "/dev/ttyS1";
  // parse the inputs
  int c = 0;
  while((c = getopt(argc, argv, "i:")) != -1)
  {
    switch(c)
    {
      case 'i':
        imu_serial_port = optarg;
        fprintf(stderr, "port: %s\n", imu_serial_port);
        break;
    }
  }
  // Test 1: open the IMU serial port
  if (!openIMUSerial(imu_serial_port, IMU_BAUD_RATE_DEFAULT))
  {
    fprintf(stderr,"Test 1: Error - Cannot connect to IMU\n");
    return -1;
  } else
    {
      printf("Test 1: Success - Connected to IMU\n");
    }
  /*// Test 2: read CBR register value
  if (!readCRBData(reg))
  {
    fprintf(stderr,"Test 2: Error - Cannot read CBR %c value\n", reg);
    return -1;
  } else
    {
      printf("Test 2: Success - Read CBR %c value\n", reg);
    }
  // Test 3: get IMU one shot mode data
  if(!getImuSensorData(&state.p, &state.q, &state.r, &state.ax, &state.ay, &state.az))
  {
    fprintf(stderr,"Test 3: Error - Cannot get one shot mode data\n");
    return -1;
  } else
    {
      printf("Test 3: Success - One shot mode data:\n");
      printf("%f,%f,%f,%f,%f,%f\n", state.p, state.q, state.r, state.ax, state.ay, state.az);
    }*/
  // Test 4: set the IMU configuration
  setIMUconfig();
  closeIMUSerial();
  openIMUSerial(imu_serial_port, IMU_BAUD_RATE);
  // Close the IMU serial port
  closeIMUSerial();
  printf("Disconnected from IMU\n");
  return 0;
}
