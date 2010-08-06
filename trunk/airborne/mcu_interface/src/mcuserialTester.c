/**
 * @file   mcuserialTestApp.c
 * @author Liam O'Sullivan
 *
 * $Author: $
 * $Date: $
 * $Rev: -1 $
 * $Id: mcuserialTestApp.c -1   $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * MCU Serial Test App for the MCU Serial Library
 * 
 */

#include <stdlib.h>
#include "mcuserial.h"

char* mcu_serial_port;

uint8_t flightMode = 1;
uint16_t commandedEngine[4];

int8_t commandedThrottle = 50;
int8_t commandedRoll = 2;
int8_t commandedPitch = 3;
int8_t commandedYaw = 4;


int main(int argc, char* argv[])
{
  mcu_serial_port = (char*) malloc(100*sizeof(char));
  mcu_serial_port = "/dev/ttyUSB0";

  int c = 0;
  while ((c = getopt(argc, argv, "i:")) != -1)
  {
    switch(c)
    {
      case 'i':
        mcu_serial_port = optarg;
        fprintf(stderr, "Port: %s\n", mcu_serial_port);
        break;
    }
  }

  if (!mcuOpenSerial(mcu_serial_port,MCU_BAUD_RATE)) // Test 1: Open
  {
    fprintf(stderr,"Test 1: Error - Unable to Open Port\n");
    return -1;
  }
  else
  {
    fprintf(stderr,"Test 1: Pass - Connected\n");
  }
  
  if (!sendMCUCommands(&flightMode, &commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 2: High Level Commands
  {
    fprintf(stderr,"Test 2: Fail - Send High Level Commands\n");
    return -1;
  }
  else
  {
    fprintf(stderr,"Test 2: Pass - Sent High Level Commands\n");
  }

  if (!getMCUPeriodic(&flightMode, commandedEngine)) // Test 3: Flight Mode and Engines
  {
    fprintf(stderr,"Test 3: Error - Unable to Get Mode and Engine Data\n");
  }
  else
  {
    fprintf(stderr,"Test 3: Pass - Got Mode and Engine Data\n");
  }
  
  if (!getMCUCommands(&commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 4: Rx High Level Commands
  {
    fprintf(stderr,"Test 4: Fail - Get High Level Commands\n");
  }
  else
  {
    fprintf(stderr,"Test 4: Pass - Get High Level Commands\n");
  }
  mcuCloseSerial();
  return 0;
}
