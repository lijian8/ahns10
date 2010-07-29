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

#include <avr/io.h>

#include "mode.h"
#include "pwm.h"
#include "modeindicator.h"

volatile enum FlightModes flightMode;

enum FlightModes rcMode; 
uint8_t rcThrottle;
uint8_t rcRoll;       
uint8_t rcPitch;       
uint8_t rcYaw;          

volatile enum FlightModes apMode;
volatile uint8_t apThrottle;
volatile uint8_t apRoll;       
volatile uint8_t apPitch;
volatile uint8_t apYaw;

static inline uint8_t Bound(uint16_t setPoint, uint8_t maxValue, uint8_t minValue);

#define HISTORY_SIZE 8
inline void MixCommands(volatile uint8_t* commandedThrottle, volatile uint8_t* commandedRoll, volatile uint8_t* commandedPitch, volatile uint8_t* commandedYaw)
{
  static int32_t escHistory[4][HISTORY_SIZE];
  static uint8_t index = 0;
  
  // Mix the Signals
  if ((*commandedThrottle) && (*commandedRoll) && (*commandedPitch) && (*commandedYaw))
  {
    escHistory[0][index] = esc1Min + *commandedThrottle + *commandedPitch - *commandedYaw;
    escHistory[1][index] = esc2Min + *commandedThrottle - *commandedRoll + *commandedYaw;
    escHistory[2][index] = esc3Min + *commandedThrottle - *commandedPitch - *commandedYaw;
    escHistory[3][index] = esc4Min + *commandedThrottle + *commandedRoll + *commandedYaw;

    // Offset the Mixed signals by min
    ESC1_COUNTER = Bound(esc1Min + MovingAverage(escHistory[0], HISTORY_SIZE), esc1Max, esc1Min);
    ESC2_COUNTER = Bound(esc2Min + MovingAverage(escHistory[1], HISTORY_SIZE), esc2Max, esc2Min);
    ESC3_COUNTER = Bound(escHistory[2][index], esc3Max, esc3Min);
    ESC4_COUNTER = Bound(esc4Min + MovingAverage(escHistory[3], HISTORY_SIZE), esc4Max, esc4Min);
    if (++index == HISTORY_SIZE)
    {
      index = 0;
    }
  }
  return;
}

inline int32_t MovingAverage(int32_t* valueArray, uint8_t arrayLength)
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

static inline uint8_t Bound(uint16_t setPoint, uint8_t maxValue, uint8_t minValue)
{
  uint8_t returnValue = setPoint;

  if(setPoint > maxValue)
  {
    returnValue = maxValue;
  }
  else if(setPoint < minValue)
  {
    returnValue = minValue;
  }
  
  return returnValue;
}
