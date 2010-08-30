/**
 * @file   arduserialTester.c
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
 * arduino serial library tester
 *
 */

#include <stdlib.h>
#include "arduserial.h"

char* arduino_serial_port;
double compassHeading = 0.0;
double batteryVoltage = 0.0;
double altitudeReading = 0.0;

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
  if (!openArduSerial(arduino_serial_port, ARDU_BAUD_RATE))
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
  // Test 3: read battery voltage
  if (!getBatteryVoltage(&batteryVoltage))
  {
    fprintf(stderr,"Test 3: Error - Cannot get battery voltage\n");
    return -1;
  } else
    {
      printf("%.1lf\nTest 3: Success - Read battery voltage\n",batteryVoltage);
    }
  // Test 4: read altitude reading
  if (!getAltitudeReading(&altitudeReading))
  {
    fprintf(stderr,"Test 4: Error - Cannot get altitude reading\n");
    return -1;
  } else
    {
      printf("%.1lf\nTest 4: Success - Read altitude sensor\n",altitudeReading);
    }
  // Test 5: read all date
  for(i=0; i<10000; i++)
  {
    if (!getCompassHeading(&compassHeading))
    {
      fprintf(stderr,"Test 5: Error - Cannot get compass heading\n");
    } else
    {
      printf("Heading: %.2lf\n",compassHeading);
    }
    if (!getBatteryVoltage(&batteryVoltage))
    {
      fprintf(stderr,"Test 5: Error - Cannot get battery voltage\n");
    } else
    {
      printf("Voltage: %.3lf\n",batteryVoltage);
    }
    if (!getAltitudeReading(&altitudeReading))
    {
      fprintf(stderr,"Test 5: Error - Cannot get altitude reading\n");
    } else
    {
      printf("Altitude: %.3lf\n",altitudeReading);
    }
  }
  // Close the arduino serial port
  closeArduSerial();
  printf("Disconnected from arduino\n");
  return 0;
}
