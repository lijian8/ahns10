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

uint8_t flightMode = 2;
uint16_t commandedEngine[4];

int8_t commandedThrottle = 0;
int8_t commandedRoll = 0;
int8_t commandedPitch = 0;
int8_t commandedYaw = 0;


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
  
  uint8_t i = 0;
  while(1)
  { 
    flightMode = 1;
    
    for (i = 0; i < 25; ++i)
    {
      commandedThrottle = (commandedThrottle + 1) % 25;
      if (sendMCUCommands(&flightMode, &commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 2: High Level Commands
      {
      fprintf(stderr,"Test 2: Pass - Sent High Level Commands\n");
      fprintf(stderr,"  Results: %d \t %u \t %u \t %u \t %u\n",flightMode,commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      if (getMCUCommands(&commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 4: Rx High Level Commands
      {
        fprintf(stderr,"  Results: %u \t %u \t %u \t %u\n",commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      else
      {
        fprintf(stderr,"Test 4: Fail - Get High Level Commands\n");
      }
    }
    
    for (i = 0; i < 25; ++i)
    {
      commandedRoll = (commandedRoll + 1) % 25;
      if (sendMCUCommands(&flightMode, &commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 2: High Level Commands
      {
      fprintf(stderr,"Test 2: Pass - Sent High Level Commands\n");
      fprintf(stderr,"  Results: %d \t %u \t %u \t %u \t %u\n",flightMode,commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      if (getMCUCommands(&commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 4: Rx High Level Commands
      {
	fprintf(stderr,"  Results: %u \t %u \t %u \t %u\n",commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      else
      {
	fprintf(stderr,"Test 4: Fail - Get High Level Commands\n");
      }
    }
    
    for (i = 0; i < 25; ++i)
    {
      commandedPitch = (commandedPitch + 1) % 25;
      if (sendMCUCommands(&flightMode, &commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 2: High Level Commands
      {
      fprintf(stderr,"Test 2: Pass - Sent High Level Commands\n");
      fprintf(stderr,"  Results: %d \t %u \t %u \t %u \t %u\n",flightMode,commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      if (getMCUCommands(&commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 4: Rx High Level Commands
      {
	fprintf(stderr,"  Results: %u \t %u \t %u \t %u\n",commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      else
      {
	fprintf(stderr,"Test 4: Fail - Get High Level Commands\n");
      }
    }

    for (i = 0; i < 25; ++i)
    {
      commandedYaw = (commandedYaw + 1) % 25;
      if (sendMCUCommands(&flightMode, &commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 2: High Level Commands
      {
      fprintf(stderr,"Test 2: Pass - Sent High Level Commands\n");
      fprintf(stderr,"  Results: %d \t %u \t %u \t %u \t %u\n",flightMode,commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      if (getMCUCommands(&commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 4: Rx High Level Commands
      {
	fprintf(stderr,"  Results: %u \t %u \t %u \t %u\n",commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
      }
      else
      {
	fprintf(stderr,"Test 4: Fail - Get High Level Commands\n");
      }
    }
  }

  /*while (1)
  {
    if (!getMCUPeriodic(&flightMode, commandedEngine)) // Test 3: Flight Mode and Engines
    {
      fprintf(stderr,"Test 3: Error - Unable to Get Mode and Engine Data\n");
    }
    else
    {
      fprintf(stderr,"Test 3: Pass - Got Mode and Engine Data\n");
      fprintf(stderr,"  Results: %d \t %u \t %u \t %u \t %u\n",flightMode,commandedEngine[0],commandedEngine[1],commandedEngine[2], commandedEngine[3]);
    }
  usleep(0.5e6);
  }
  
  if (!getMCUCommands(&commandedThrottle, &commandedRoll, &commandedPitch, &commandedYaw)) // Test 4: Rx High Level Commands
  {
    fprintf(stderr,"Test 4: Fail - Get High Level Commands\n");
  }
  else
  {
    fprintf(stderr,"Test 4: Pass - Get High Level Commands\n");
    fprintf(stderr,"  Results: %u \t %u \t %u \t %u\n",commandedThrottle,commandedRoll,commandedPitch,commandedYaw);
  }*/
  mcuCloseSerial();
  return 0;
}
