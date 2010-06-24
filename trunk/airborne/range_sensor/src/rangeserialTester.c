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
 * Range sensor serial library tester
 *
 */
#include <stdlib.h>
#include "rangeserial.h"

char* range_serial_port;
double zRange;
int i=0;

int main(int argc, char* argv[])
{
  // initialise the range_serial_port array
  range_serial_port = (char *) malloc(100*sizeof(char));
  // default range sensor serial port
  range_serial_port = "/dev/ttyUSB0";
  // parse the inputs
  int c = 0;
  while((c = getopt(argc, argv, "i:")) != -1)
  {
    switch(c)
    {
      case 'i':
        range_serial_port = optarg;
        fprintf(stderr, "port: %s\n", range_serial_port);
        break;
    }
  }
  // Test 1: open the Range serial port
  if (!openSerial(range_serial_port, BAUD_RATE_DEFAULT))
  {
    fprintf(stderr,"Test 1: Error - Cannot connect to Range sensor\n");
    return -1;
  } else
    {
      printf("Test 1: Success - Connected to Range sensor\n");
    }
  // Test 2: get Range serial data
  for(i=0; i<999999; i++)
  {
    if(!getRangeData(&zRange))
    {
      fprintf(stderr,"Test 2: Error - Cannot get Range sensor data\n");
      return -1;
    } else
      {
        printf("%d:%f\n", i,zRange);
      }
  }
  if (i=10)
  {
    printf("Test 2: Success - Range sensor data\n");
  }
  // Close the Range serial port
  closeSerial();
  printf("Disconnected from Range sensor\n");
  return 0;
}
