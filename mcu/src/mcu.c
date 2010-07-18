/**
 * \file   mcu.c
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
  stdout = &debugOut;
  printf("System Initialised\n");
  

  StartPWM();

  for ( ; ; )
  {
    ProcessPC();
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

  sei();
  return;
}
