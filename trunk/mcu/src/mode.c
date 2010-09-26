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

volatile enum FlightModes apMode = RC_NONE;
volatile int8_t apThrottle;
volatile int8_t apRoll;       
volatile int8_t apPitch;
volatile int8_t apYaw;

volatile int8_t commandedThrottle;
volatile int8_t commandedRoll;
volatile int8_t commandedPitch;
volatile int8_t commandedYaw;

static void CheckAPFailSafe();
static void GiveRC();

#define HISTORY_SIZE 8
inline void MixCommands()
{
  static int16_t escHistory[4][HISTORY_SIZE];
  static uint8_t index = 0;
  int8_t setPoint = 0;
  uint8_t i = 0;
 
  // Mix the Signals
  if (commandedThrottle)
  {
    escHistory[0][index] = commandedThrottle + commandedPitch - commandedYaw;
    escHistory[1][index] = commandedThrottle - commandedRoll + commandedYaw;
    escHistory[2][index] = commandedThrottle - commandedPitch - commandedYaw;
    escHistory[3][index] = commandedThrottle + commandedRoll + commandedYaw;

    // Bound and Output
    for (i = 0; i < 4; ++i)
    {
      // Bound
      if(escHistory[i][index] > PWMToCounter(2000))
      {
        //setPoint = escLimits[i][1];
        escHistory[i][index] = PWMToCounter(2000);
      }
      else if(escHistory[i][index] < PWMToCounter(1000))
      {
        //setPoint = escLimits[i][0];
        escHistory[i][index] = PWMToCounter(1000);
      }
      setPoint = escHistory[i][index];
      // Moving Average
      //setPoint = MovingAverage(escHistory[i],HISTORY_SIZE);
      
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
  int32_t sum = 0;
  int16_t average = 0;
  
  for (i = 0; i < arrayLength; ++i)
  {
    sum += valueArray[i];
  }

  average = sum / arrayLength;

  return average;
}

inline void CombineCommands()
{
  // Flight Mode Choice limited to RC
  flightMode = rcMode;
  
  switch (flightMode)
  {
    case MANUAL_DEBUG:
      IndicateManual();
#ifdef _GYRO_
      // Pass through for use with gyro
      // In this case gyro will do mixing
      ESC1_COUNTER = PWMToCounter(zeroThrottle) + rcThrottle;
      ESC2_COUNTER = PWMToCounter(zeroRoll) + rcRoll;
      ESC3_COUNTER = PWMToCounter(zeroPitch) + rcPitch;
      ESC4_COUNTER = PWMToCounter(zeroYaw) - rcYaw;
#else
      commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
      commandedRoll = rcRoll;
      commandedPitch = rcPitch;
      commandedYaw = rcYaw;
      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);
#endif
      break;
    case AUGMENTED: // Combine AP and RC Commands
      //CheckAPFailSafe();
      IndicateAugmented(); 
#ifdef _GYRO_ 
      // Combine AP Guidance Loop and RC pulses then let gyro mix
      commandedThrottle = PWMToCounter(zeroThrottle) + apThrottle + rcThrottle;
      commandedRoll = PWMToCounter(zeroRoll) + apRoll + rcRoll;
      commandedPitch = PWMToCounter(zeroPitch) + apPitch + rcPitch;
      commandedYaw = PWMToCounter(zeroYaw) - apYaw - rcYaw;
      GiveRC();
      ESC1_COUNTER = commandedThrottle;
      ESC2_COUNTER = commandedRoll;
      ESC3_COUNTER = commandedPitch;
      ESC4_COUNTER = commandedYaw;
#else
      // Use the RC Commands as setpoints and combine these in the FC
      commandedThrottle = PWMToCounter(zeroThrottle) + apThrottle;
      commandedRoll = apRoll;
      commandedPitch = apPitch;
      commandedYaw = apYaw;
      GiveRC();
      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);
#endif
      break;
    case AUTOPILOT: // Pass AP unaltered
      //CheckAPFailSafe();
      IndicateAutopilot();
#ifdef _GYRO_ 
      // Let Gyro Mix the AP Guidance Commands 
      commandedThrottle = PWMToCounter(zeroThrottle) + apThrottle;
      commandedRoll = PWMToCounter(zeroRoll) + apRoll;
      commandedPitch = PWMToCounter(zeroPitch) + apPitch;
      commandedYaw = PWMToCounter(zeroYaw) - apYaw;
      GiveRC();
      ESC1_COUNTER = commandedThrottle;
      ESC2_COUNTER = commandedRoll;
      ESC3_COUNTER = commandedPitch;
      ESC4_COUNTER = commandedYaw;
#else
      // RC Commands still sent to FC but not used
      commandedThrottle = PWMToCounter(zeroThrottle) + apThrottle;
      commandedRoll = apRoll;
      commandedPitch = apPitch;
      commandedYaw = apYaw;
      GiveRC();
      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);
#endif
      break;
    default:
      commandedThrottle = PWMToCounter(zeroThrottle);
      commandedRoll = 0;
      commandedPitch = 0;
      commandedYaw = 0;
      MixCommands(commandedThrottle, commandedRoll, commandedPitch, commandedYaw);
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

static const double controlFactor = 1;
static inline void GiveRC()
{
#ifdef _GYRO_
      switch (apMode)
      {
        case RC_THROTTLE:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          break;
        case RC_ROLL:
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          break;
        case RC_PITCH:
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          break;
        case RC_YAW:
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_THROTTLE_ROLL:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          break;
        case RC_THROTTLE_PITCH:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          break;
        case RC_THROTTLE_YAW:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_ROLL_PITCH:
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          break;
        case RC_ROLL_YAW:
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_PITCH_YAW:
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_THROTTLE_ROLL_PITCH:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          break;
        case RC_THROTTLE_ROLL_YAW:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_THROTTLE_PITCH_YAW:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_ROLL_PITCH_YAW:
          commandedRoll = PWMToCounter(zeroRoll) + controlFactor*rcRoll;
          commandedPitch = PWMToCounter(zeroPitch) + controlFactor*rcPitch;
          commandedYaw = PWMToCounter(zeroYaw) + controlFactor*rcYaw;
          break;
        case RC_NONE:
          break;
        case FAIL_SAFE:
          failSafe = 1;
          break;
      }
#else
      switch (apMode)
      {
        case RC_THROTTLE:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          break;
        case RC_ROLL:
          commandedRoll = controlFactor*rcRoll;
          break;
        case RC_PITCH:
          commandedPitch = controlFactor*rcPitch;
          break;
        case RC_YAW:
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_THROTTLE_ROLL:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedRoll = controlFactor*rcRoll;
          break;
        case RC_THROTTLE_PITCH:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedPitch = controlFactor*rcPitch;
          break;
        case RC_THROTTLE_YAW:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_ROLL_PITCH:
          commandedRoll = controlFactor*rcRoll;
          commandedPitch = controlFactor*rcPitch;
          break;
        case RC_ROLL_YAW:
          commandedRoll = controlFactor*rcRoll;
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_PITCH_YAW:
          commandedPitch = controlFactor*rcPitch;
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_THROTTLE_ROLL_PITCH:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedRoll = controlFactor*rcRoll;
          commandedPitch = controlFactor*rcPitch;
          break;
        case RC_THROTTLE_ROLL_YAW:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedRoll = controlFactor*rcRoll;
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_THROTTLE_PITCH_YAW:
          commandedThrottle = PWMToCounter(zeroThrottle) + rcThrottle;
          commandedPitch = controlFactor*rcPitch;
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_ROLL_PITCH_YAW:
          commandedRoll = controlFactor*rcRoll;
          commandedPitch = controlFactor*rcPitch;
          commandedYaw = controlFactor*rcYaw;
          break;
        case RC_NONE:
          break;
        case FAIL_SAFE:
          failSafe = 1;
          break;
      }
#endif
}
