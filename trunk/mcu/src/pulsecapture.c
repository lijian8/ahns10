/**
 * \file   pulsecapture.c
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-06-10 23:59:05 +1000 (Thu, 10 Jun 2010) $
 * $Rev: 164 $
 * $Id: main.cpp 164 2010-06-10 13:59:05Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Implementation of mode control unit RC PWM Pulse Capture
 */

#include "avrdefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "pulsecapture.h"


volatile Channel inputChannel[NUM_CHANNELS];
volatile uint8_t risenPINC;
volatile uint8_t fallenPINC;
volatile uint8_t countChanged;

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
  // CS2_210 = 010 for Prescale or 8 to 1MHz
  //         = 100 for 64 to 125kHz
  TCCR2B = (0 << WGM22) | (1 << CS22) | (0 << CS21) | (0 << CS20);

  // Interrupt Mask
  // TOIE2 - for Timer overflow
  //TIMSK2 = 0;
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

void ProcessPC()
{
  uint8_t i = 0;
  if ((risenPINC != 0) || (fallenPINC != 0))  // process input capture
  {
    for (i = 0; i < NUM_CHANNELS; ++i)
    {
      if (BRS(risenPINC,i)) // channel i changed to high
      {
        risenPINC &= CBR(i);

        if (!inputChannel[i].isHigh) // not known high - ok
        {
          inputChannel[i].isHigh = 1;
          inputChannel[i].startTimerCount = countChanged;
        }
        else // known high - lost falling edge
        {
          inputChannel[i].isHigh = 1;
          inputChannel[i].startTimerCount = countChanged;
          inputChannel[i].overflowCount = 0;
        }
      }
      else if (BRS(fallenPINC,i)) // channel i changed to low
      {
        fallenPINC &= CBR(i);
        if (inputChannel[i].isHigh) // not known low so tracked - ok
        {
          inputChannel[i].isHigh = 0;
          inputChannel[i].measuredPulseWidth = 8.0*((256 - inputChannel[i].startTimerCount) + (inputChannel[i].overflowCount - 1)*256 + countChanged);
          inputChannel[i].startTimerCount = 0;
	  inputChannel[i].overflowCount = 0;
          
	  // DEBUG
	  printf("Channel %i: %lu \t",i,inputChannel[i].measuredPulseWidth);


	  /*if ( i == CHANNEL1 )
	  {
	  	ESC1_COUNTER = SetPWM(inputChannel[CHANNEL1].measuredPulseWidth);
	  }*/
        }
        //else known low - missed rising edge ignore
       }
     }
   }

  return;
}

/**
 * @brief Interrupt called due to changes on any of the channel ports
 * 
 * Determines those channels changed, if they were rising or falling
 * and logs the time that these changed.
 * Updated data is processes by ProcessPC() later.
 */
ISR(PCINT1_vect)
{
  static uint8_t previousPINC = 0;
  uint8_t changedPINC = 0;
  uint8_t i = 0;

  // Get Changed Time
  countChanged = TCNT2;

  // Flag the Changes
  changedPINC = PINC ^ previousPINC;
  
  // Determine Falling or Rising Edge
  // TODO Only Check Changed Channels
  for (i = 0; i < NUM_CHANNELS; ++i)
  {
    if (BRS(changedPINC,i))
    {
      //PORTD ^= (1 << RED_LED);
      if (BRS(previousPINC,i)) // previously high
      {
        fallenPINC |= SBR(i);
	risenPINC &= CBR(i);
      }
      else // previously low
      {
        risenPINC |= SBR(i);
	fallenPINC &= CBR(i);
      }
    }
  }
  // Update Previous
  previousPINC = PINC;
}

/**
 * @brief Interrupt called when Timer2 Overflows to log time
 *
 * In the RC pulse capturing 1MHz cannot provide non-overflow timing 
 * thus channels being timed need to track the number of overflows too. 
 */
ISR(TIMER2_OVF_vect)
{
  uint8_t i = 0;
  
  // Increment Timer Overflow Counts of High Pulses
  for (i = 0; i < NUM_CHANNELS; ++i)
  {
    if (inputChannel[i].isHigh)
    {
      inputChannel[i].overflowCount++;
    }
  }
}

