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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "pwm.h"
#include "pulsecapture.h"
#include "usart.h"
#include "modeindicator.h"

/**
 * @brief Pack of all Initialise Routines
 */
void init();

void main (void)
{
  StopPWM();
  
  init();
  sei();
  _delay_ms(250);


  // Initialise and detect the Inputs
  #ifndef DEBUG
  uint8_t inputsInitialised = 0;
  do 
  {
    ProcessPC();
    if ((inputChannel[inputsInitialised].measuredPulseWidth < PWM_MAX_US) && (inputChannel[inputsInitialised].measuredPulseWidth > PWM_MIN_US))
    {
      inputsInitialised++;
    }
    else
    {
      ToggleRed(TOGGLE);
      _delay_ms(250);
    }
  } while (inputsInitialised < NUM_CHANNELS);
  ToggleGreen(ON);
  ToggleRed(OFF);
  #endif

  stdout = &debugOut;
  printf("System Initialised\n");
  StartPWM();

  for ( ; ; )
  {
    // Process USART Commands all in interrupts

    // Pulse Capture
    ProcessPC();

    // Mix RC and USART inputs
    mixCommands();

    //ESC1_COUNTER = COMMANDED_COLLECTIVE + COMMANDED_PITCH - COMMANDED_YAW;
    //ESC2_COUNTER = COMMANDED_COLLECTIVE - COMMANDED_ROLL + COMMANDED_YAW;
    //ESC3_COUNTER = COMMANDED_COLLECTIVE - COMMANDED_PITCH - COMMANDED_YAW;
    //ESC4_COUNTER = COMMANDED_COLLECTIVE + COMMANDED_ROLL + COMMANDED_YAW;

    // Output Mixed PWM Signals

    
  }

  return;
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
