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
#include "MCUCommands.h"

char* mcu_serial_port;

uint8_t flightMode = AUTOPILOT;


int8_t commandedThrottle = 0;
int8_t commandedRoll = 0;
int8_t commandedPitch = 0;
int8_t commandedYaw = 0;

int8_t readThrottle = 0;
int8_t readRoll = 0;
int8_t readPitch = 0;
int8_t readYaw = 0;

uint8_t readFlightMode = RC_NONE;
uint16_t readEngine[4];


int main(int argc, char* argv[])
{
  mcu_serial_port = (char*) malloc(100*sizeof(char));
  mcu_serial_port = "/dev/ttyS0";

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
  
  int8_t i = 0;
  while(1)
  { 
    for (i = 0; i < 100; i = i + 4)
    {
      usleep(1e6);
      commandedThrottle = i;
      commandedRoll = i/5;
      commandedPitch = i/4;
      commandedYaw = i/2;
      
      // Test 2: Send High Level Commands
      if (sendMCUCommands(&flightMode, &commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) 
      {
      fprintf(stderr,"Test 2: Pass - Sent High Level Commands\n");
      fprintf(stderr,"  Results: %d \t %d \t %d \t %d \t %d\n",flightMode,commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
    
      // Test 3: Receive High Level Commands
      if (!getMCUCommands(&readThrottle, &readRoll, &readPitch, &readYaw)) 
      {
        fprintf(stderr,"Test 3: Error - Get High Level Commands\n");
      }
      else
      {
        fprintf(stderr,"Test 3: Pass - Got High Level Commands\n");
        fprintf(stderr,"  Results: %d \t %d \t %d \t %d\n",readThrottle, readRoll, readPitch, readYaw);
      }

      // Test 4: Receive Engine and Flight Mode
      if (!getMCUPeriodic(&readFlightMode, readEngine)) 
      {
        fprintf(stderr,"Test 4: Error - Get Engine Commands\n");
      }
      else
      {
        fprintf(stderr,"Test 4: Pass - Got Engine Commands\n");
        fprintf(stderr,"  Results: %d \t %u \t %u \t %u \t %u\n", readFlightMode, readEngine[0], readEngine[1], readEngine[2], readEngine[3]);
      }
    }
  }
  mcuCloseSerial();
  return 0;
}
