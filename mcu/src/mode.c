/**
 * \file   mode.c
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-07-20 15:27:52 +1000 (Tue, 20 Jul 2010) $
 * $Rev: 248 $
 * $Id: usart.h 248 2010-07-20 05:27:52Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Implementation of MCU esc mixing modes
 */

#include "avrdefines.h"
#include "MCUCommands.h"

#include <avr/io.h>

#include "mode.h"
#include "pwm.h"
#include "usart.h"
#include "pulsecapture.h"
#include "modeindicator.h"

volatile enum FlightModes flightMode;
volatile uint8_t failSafe = 0;

enum FlightModes rcMode; 
int8_t rcThrottle;
int8_t rcRoll;       
int8_t rcPitch;       
int8_t rcYaw;          

volatile enum FlightModes apMode;
volatile int8_t apThrottle;
volatile int8_t apRoll;       
volatile int8_t apPitch;
volatile int8_t apYaw;

volatile int8_t commandedThrottle;
volatile int8_t commandedRoll;
volatile int8_t commandedPitch;
volatile int8_t commandedYaw;

static void CheckAPFailSafe();

#define HISTORY_SIZE 8
inline void MixCommands()
{
  static int32_t escHistory[4][HISTORY_SIZE];
  static uint8_t index = 0;
  int8_t setPoint = 0;
  uint8_t i = 0;
 
  // Mix the Signals
  if (commandedThrottle)
  {
    escHistory[0][index] = escLimits[0][0] + commandedThrottle + commandedPitch - commandedYaw;
    escHistory[1][index] = escLimits[1][0] + commandedThrottle - commandedRoll + commandedYaw;
    escHistory[2][index] = escLimits[2][0] + commandedThrottle - commandedPitch - commandedYaw;
    escHistory[3][index] = escLimits[3][0] + commandedThrottle + commandedRoll + commandedYaw;

    // Bound and Output
    for (i = 0; i < 4; ++i)
    {
      if(escHistory[i][index] > escLimits[i][1])
      {
        setPoint = escLimits[i][1];
      }
      else if(escHistory[i][index] < escLimits[i][0])
      {
        setPoint = escLimits[i][0];
      }
      else
      {
        setPoint = escHistory[i][index];
      }
      
      switch (i)
      {
        case 0:
          ESC1_COUNTER = setPoint;
          break;
        case 1:
          ESC2_COUNTER = setPoint;
          break;
        case 2:
          ESC3_COUNTER = setPoint;
          break;
        case 3:
          ESC4_COUNTER = setPoint;
      }
    }
    if (++index == HISTORY_SIZE)
    {
      index = 0;
    }
  }
  return;
}

inline int16_t MovingAverage(int16_t* valueArray, uint8_t arrayLength)
{
  uint8_t i = 0;
  int32_t sum = 0, average = 0;
  
  for (i = 0; i < arrayLength; ++i)
  {
    sum += valueArray[i];
  }

  average = sum / arrayLength;

  return average;
}

inline void CombineCommands()
{
  // Only Throttle has Full Authority
  const double controlFactor = 0.1;
  
  // Flight Mode Choice limited to RC
  flightMode = rcMode;
  
  switch (flightMode)
  {
    case MANUAL_DEBUG:
      IndicateManual();

      commandedThrottle = rcThrottle;
      commandedRoll = controlFactor*rcRoll;
      commandedPitch = controlFactor*rcPitch;
      commandedYaw = controlFactor*rcYaw;
      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);

      break;
    case AUGMENTED: // Combine AP and RC Commands
      IndicateAugmented();
      CheckAPFailSafe();
 
      commandedThrottle = 0.5*rcThrottle + 0.5*apThrottle;
      commandedRoll = 0.5*controlFactor*rcRoll + 0.5*apRoll;
      commandedPitch = 0.5*controlFactor*rcPitch + 0.5*apPitch;
      commandedYaw = 0.5*controlFactor*rcYaw + 0.5*apYaw; 
      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);

      break;
    case AUTOPILOT: // Pass AP unaltered
      IndicateAutopilot();
      CheckAPFailSafe();

      commandedThrottle = apThrottle;
      commandedRoll = apRoll;
      commandedPitch = apPitch;
      commandedYaw = apYaw;

      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);
      break;
    default:
      commandedThrottle = 0;
      commandedRoll = 0;
      commandedPitch = 0;
      commandedYaw = 0;
      break;
  }
  return;
}

static inline void CheckAPFailSafe()
{
  if (micro() - apLastCommands > 40000) // 0.04 second drop (25Hz)
  {
    failSafe = 1;
  }

  return;
}
