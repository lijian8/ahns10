/**
 * \file   pulsecapture.c
 * \author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Implementation of mode control unit RC PWM Pulse Capture
 */

#include "avrdefines.h"
#include "MCUCommands.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "pulsecapture.h"
#include "modeindicator.h"
#include "mode.h"
#include "pwm.h"

volatile Channel inputChannel[NUM_CHANNELS];

const uint8_t PC_DT_US = (1e6*64.0/F_CPU);

volatile uint8_t newRC = 0;

uint16_t zeroThrottle = 1250;
uint16_t zeroRoll = 1500;
uint16_t zeroPitch = 1500; 
uint16_t zeroYaw = 1500;

/** @brief Timer 2 Overflow Counter */
static volatile uint32_t timer2OverflowCount = 0;

uint8_t InitialiseTimer2()
{
  // Clear Asynchronous Source
  ASSR = 0;

  // Register A
  // COM2A_10 = 00 for normal mode
  // WGM2_210 = 000 for normal mode
  TCCR2A = (0 << COM2A1) | (0 << COM2A0) | 
           (0 << WGM21) | (0 << WGM20);

  // Register B
  // CS2_210 = 010 for Prescale or 8 to 1MHz or 1us
  //         = 011 for 32 to 250kHz or 4us
  //         = 100 for 64 to 125kHz or 8us
  TCCR2B = (0 << WGM22) | (1 << CS22) | (0 << CS21) | (0 << CS20);

  // Interrupt Mask
  // TOIE2 - for Timer overflow
  TIMSK2 |= (1 << TOIE2);

  return 1;
}

uint8_t InitialisePC()
{
  // Pins to Inputs to enable read of high or low
  DDRC = (0 << CHANNEL1) | (0 << CHANNEL2) | (0 << CHANNEL3) | (0 << CHANNEL4) | (0 << CHANNEL5) | (0 << CHANNEL6);
 
  // PC Interrupt Control Reg
  PCICR = (1 << PCIE1);

  // PC Interrupt MASK
  // Interrupts for the Pins all in PCMSK1
  PCMSK1 = (1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8);
   
  return 1;
}

#define SWITCH_HISTORY_SIZE 16
inline void UpdateRC()
{
  static int16_t modeHistory[SWITCH_HISTORY_SIZE];
  static int16_t armHistory[SWITCH_HISTORY_SIZE];
  static int8_t index = 0;
  
  // Map Input Channels to RC Channel
  uint16_t armPulse = inputChannel[CHANNEL1].measuredPulseWidth; // mode pulse
  uint16_t modePulse = inputChannel[CHANNEL2].measuredPulseWidth;
  uint16_t yawPulse = inputChannel[CHANNEL3].measuredPulseWidth;
  uint16_t pitchPulse = inputChannel[CHANNEL4].measuredPulseWidth;
  uint16_t rollPulse = inputChannel[CHANNEL5].measuredPulseWidth;
  uint16_t throttlePulse = inputChannel[CHANNEL6].measuredPulseWidth;
 
  // Store Arm and Mode Switch Input
  armHistory[index] = armPulse;
  modeHistory[index] = modePulse;
  
  // Average Switches
  modePulse = MovingAverage(modeHistory,SWITCH_HISTORY_SIZE);
  armPulse = MovingAverage(armHistory,SWITCH_HISTORY_SIZE);
  
  // Determine AP Mode
  if (throttlePulse < (1100)) // failsafe
  {
    failSafe = 1;
  }
  else if (armPulse > (PC_PWM_MAX - PULSE_TOLERANCE)) // autopilot armed
  {
    if (modePulse > (PC_PWM_MAX - PULSE_TOLERANCE))
    {
      rcMode = AUTOPILOT;
    }
    else if (modePulse > (PC_PWM_MIN - PULSE_TOLERANCE))
    {
      rcMode = AUGMENTED;
    }
  }
  else
  {
    rcMode = MANUAL_DEBUG;
  }

  rcThrottle = PWMToCounter(throttlePulse - zeroThrottle);
  if (rcThrottle < 5)
  {
    rcRoll = 0;
    rcPitch = 0;
    rcYaw = 0;
  }
  else 
  {
    rcRoll = PWMToCounter(-(rollPulse - zeroRoll));
    rcPitch = PWMToCounter(-(pitchPulse - zeroPitch));
    rcYaw = PWMToCounter(yawPulse - zeroYaw);
  }
  
  if (++index == SWITCH_HISTORY_SIZE)
  {
    index = 0;
  }

  #ifdef DEBUG
    //printf("%u\n", throttlePulse);
    //printf("%u\n", modePulse);
    //printf("%u\n", zeroThrottle);
  #endif
  return;
}

/**
 * @brief Interrupt called due to changes on any of the channel ports
 * 
 * Determines those channels changed, if they were rising or falling
 * and logs the time that these changed.
 */
ISR(PCINT1_vect)
{
  static uint8_t previousPINC = 0;
  uint8_t changedPINC = 0;
  uint8_t i = 0;
  uint16_t tempPulse = 0;

  // Time Changed 
  uint32_t timeChanged = micro();

  // Flag the Changes
  changedPINC = PINC ^ previousPINC;
  
  // Determine Falling or Rising Edge
  for (i = 0; i < NUM_CHANNELS; ++i)
  {
    if (BRS(changedPINC,i))
    {
      //PORTD ^= (1 << RED_LED);
      if (BRS(previousPINC,i)) // fallen
      {
        if(inputChannel[i].isHigh) // got edge
        { 
          newRC = 1;
          inputChannel[i].isHigh = 0;
          tempPulse = timeChanged - inputChannel[i].startTime;
          if ((tempPulse > PC_PWM_MIN) && (tempPulse < PC_PWM_MAX)) // update only with valid pulse
          {
            inputChannel[i].measuredPulseWidth = tempPulse;
          }
        }
      }
      else // risen
      {
        inputChannel[i].isHigh = 1;
        inputChannel[i].startTime = timeChanged;
      }
    }
  }
  // Update Previous
  previousPINC = PINC;
}

/**
 * @brief Interrupt called when Timer2 Overflows to log time
 *
 * In the RC pulse capturing 125kHz cannot provide non-overflow timing 
 * thus channels being timed need to track the number of overflows too. 
 */
ISR(TIMER2_OVF_vect)
{
  // log the overflow
  timer2OverflowCount++;
}

inline uint32_t micro()
{
  return PC_DT_US*(TCNT2 + (timer2OverflowCount*256));
}
