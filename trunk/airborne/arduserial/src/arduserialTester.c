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
#include "arduserial.h"

char* arduino_serial_port;
double compassHeading = 0.0;

int main(int argc, char* argv[])
{
  // initialise the imu_serial_port array
  arduino_serial_port = (char *) malloc(100*sizeof(char));
  // default arduino serial port
  arduino_serial_port = "/dev/ttyUSB0";
  // counter variable
  int i = 0;
  // parse the inputs
  int c = 0;
  while((c = getopt(argc, argv, "i:")) != -1)
  {
    switch(c)
    {
      case 'i':
        arduino_serial_port = optarg;
        fprintf(stderr, "port: %s\n", arduino_serial_port);
        break;
    }
  }
  // Test 1: open the arduino serial port
  if (!openArduSerial(arduino_serial_port, BAUD_RATE))
  {
    fprintf(stderr,"Test 1: Error - Cannot connect to the arduino\n");
    return -1;
  } else
    {
      printf("Test 1: Success - Connected to arduino\n");
    }
  // Test 2: read compass heading
  if (!getCompassHeading(&compassHeading))
  {
    fprintf(stderr,"Test 2: Error - Cannot get compass heading\n");
    return -1;
  } else
    {
      printf("%.1lf\nTest 2: Success - Read compass heading\n",compassHeading);
    }
  // Test 3: read 10 compass heading
  for(i=0; i<10; i++)
  {
    if (!getCompassHeading(&compassHeading))
    {
      fprintf(stderr,"Test 3: Error - Cannot get compass heading\n");
      return -1;
    } else
    {
      printf("%.1lf\n",compassHeading);
    }
  }
  if (i == 10)
  {
    printf("Test 3: Success - Read 10 compass headings\n");
  }
  // Close the arduino serial port
  closeArduSerial();
  printf("Disconnected from arduino\n");
  return 0;
}
