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

volatile enum FlightModes rcMode; 
uint8_t rcThrottle;
uint8_t rcRoll;       
uint8_t rcPitch;       
uint8_t rcYaw;          

volatile enum FlightModes apMode;
volatile uint8_t apThrottle;
volatile uint8_t apRoll;       
volatile uint8_t apPitch;
volatile uint8_t apYaw;


static inline uint8_t MovingAverage(uint8_t* valueArray, const uint16_t arrayLength, const uint8_t startPosition, const uint8_t pointCount);
static inline uint8_t Bound(uint8_t setPoint, const uint8_t counterMax, const uint8_t counterMin);

inline void CombineCommands()
{
  // Flight Mode Choice limited to RC
  flightMode = rcMode;

  if (flightMode == MANUAL_DEBUG) // Pass RC unaltered
  {
    IndicateManual();
    MixCommands(rcThrottle, rcRoll, rcPitch, rcYaw);
  }
  else if (flightMode == AUGMENTED) // Combine AP and RC Commands
  {
    IndicateAugmented();
    /** TODO IMPELEMENT THE AUGMENTATION */
  }
  else if (flightMode == AUTOPILOT) // Pass AP unaltered
  {
    IndicateAutopilot();
    MixCommands(apThrottle, apRoll, apPitch, apYaw);
  }
  else // mode not known
  {
    MixCommands(0,0,0,0);
  }

  return;
}

inline void MixCommands(uint8_t commandedThrottle, uint8_t commandedRoll, uint8_t commandedPitch, uint8_t commandedYaw)
{
  static uint8_t escHistory[4][500];
  static uint16_t index = 0;
  static uint8_t esc1Min = 0, esc1Max = 0;
  static uint8_t esc2Min = 0, esc2Max = 0;
  static uint8_t esc3Min = 0, esc3Max = 0;
  static uint8_t esc4Min = 0, esc4Max = 0;

  esc1Min = PWMToCounter(ESC1_MIN);
  esc1Max = PWMToCounter(ESC1_MAX);
  
  esc2Min = PWMToCounter(ESC2_MIN);
  esc2Max = PWMToCounter(ESC2_MAX);
  
  esc3Min = PWMToCounter(ESC3_MIN); 
  esc3Max = PWMToCounter(ESC3_MAX);
  
  esc4Min = PWMToCounter(ESC4_MIN); 
  esc4Max = PWMToCounter(ESC4_MAX);

  // Mix the Signals
  escHistory[0][index] = commandedThrottle + commandedPitch - commandedYaw;
  escHistory[1][index] = commandedThrottle - commandedRoll + commandedYaw;
  escHistory[2][index] = commandedThrottle - commandedPitch - commandedYaw;
  escHistory[3][index] = commandedThrottle + commandedRoll + commandedYaw;

  // Offset the Mixed signals by min
  ESC1_COUNTER = Bound(esc1Min + MovingAverage(escHistory[0], 500, index, 5), esc1Max, esc1Min);
  ESC2_COUNTER = Bound(esc2Min + MovingAverage(escHistory[1], 500, index, 5), esc2Max, esc2Min);
  ESC3_COUNTER = Bound(esc3Min + MovingAverage(escHistory[2], 500, index, 5), esc3Max, esc3Min);
  ESC4_COUNTER = Bound(esc4Min + MovingAverage(escHistory[3], 500, index, 5), esc4Max, esc4Min);
  
  index = (index + 1) % 500;
  return;
}

static inline uint8_t Bound(uint8_t setPoint, const uint8_t counterMax, const uint8_t counterMin)
{
  // Max Bound
  if (setPoint > counterMax)
  {
    setPoint = counterMax;
  }
  else if (setPoint < counterMin) // Min Bound
  {
    setPoint = counterMin;
  }

  return setPoint;
}

static inline uint8_t MovingAverage(uint8_t* valueArray, const uint16_t arrayLength, const uint8_t startPosition, const uint8_t pointCount) 
{
  uint16_t index = startPosition, count = 0;
  uint32_t sum = 0, average = 0;

  do
  {
    sum += valueArray[index];
    count++;
    index--;
    if (index < 0)
    {
      index = arrayLength - 1;
    }

  } while(count < pointCount);

  average = sum / pointCount;

  return average;
}
