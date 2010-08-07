/**
 * \file   mcu.c
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
 * Main file for the ATMEGA328P Mode Control Unit
 */

#include "avrdefines.h"
#include "MCUCommands.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "pwm.h"
#include "pulsecapture.h"
#include "usart.h"
#include "modeindicator.h"
#include "mode.h"

/**
 * @brief Pack of all Initialise Routines
 */
void init();

/**
 * @brief Combine High Level Commands
 *
 * Depending on the selected flight mode:
 * 
 * - Pass the RC Commands without modification
 * - Augment the RC commands with AP commands
 * - Pass the AP Commands without modification 
 */
void CombineCommands();


int main (void)
{
  uint8_t inputsInitialised = 0;
  StopPWM();
  init();
  #ifdef DEBUG
  stdout = &debugOut;
  printf("System Initialised\n");
  #endif
  sei();
  _delay_ms(250);


  // Initialise and detect the Inputs
  do 
  {
    if (inputChannel[inputsInitialised].measuredPulseWidth < (PC_PWM_MIN + PULSE_TOLERANCE))
    {
      //printf("%u\n",inputChannel[inputsInitialised].measuredPulseWidth);
      inputsInitialised++;
    }
    else
    {
      ToggleRed(TOGGLE);
      _delay_ms(250);
    }
  } while (inputsInitialised < NUM_CHANNELS);

  // Calibrate Neutral Pulse Widths
  _delay_ms(1000);
  
  // Minimum Output
  ESC1_COUNTER = escLimits[0][0];
  ESC2_COUNTER = escLimits[1][0];
  ESC3_COUNTER = escLimits[2][0];
  ESC4_COUNTER = escLimits[3][0];
  StartPWM();
  
  for ( ; ; )
  {
    // RC Pulse Capture
    if(newRC) // new RC values to process
    {
      newRC = 0;
      UpdateRC();
      if(failSafe) // Never Recover from Failsafe 
      {
        static uint8_t toggleEnabled = 0;
        
        // Minimum Commands
  	ESC1_COUNTER = escLimits[0][0];
	ESC2_COUNTER = escLimits[1][0];
	ESC3_COUNTER = escLimits[2][0];
        ESC4_COUNTER = escLimits[3][0];
  
        if (!toggleEnabled) // prepare to go into failsafe
        {
          toggleEnabled = 1;
          ToggleRed(OFF);
          ToggleGreen(OFF);
	  flightMode = FAIL_SAFE;
	}
        else  // Toggle Yellow Error at 10Hz
        {
	  _delay_ms(100);
          ToggleGreen(TOGGLE);
          ToggleRed(TOGGLE);
        }
      }
      else
      {	      
        // Combine RC and AP inputs
        // Then Output them
        newAPCommands = 0;
        CombineCommands();
      }
    }
    else if (newAPCommands) // only new autopilot commands
    {
      newAPCommands = 0;
      CombineCommands();
    }

    // Communicate with Flight Computer
    if (txCommands)
    {
      txCommands = 0;
      USARTtxCommands();
    }

    if (txPeriodic)
    {
      txPeriodic = 0;
      USARTtxPeriodic();
    }
  }
  return 0;
}


inline void init()
{
  // Setup Clock - prescalar of 2 to 8MHz
  CLKPR = (1<<CLKPCE);
  CLKPR = (1<<CLKPS0);
  
  InitialiseModeIndicator();
  InitialiseTimer0();
  InitialiseTimer1();

  InitialiseUSART();

  InitialiseTimer2();
  InitialisePC();

  return;
}

