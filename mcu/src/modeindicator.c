/**
 * \file   modeindicator.c
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
 * Implementation of Mode Indicator functions
 */

#include "avrdefines.h"

#include <avr/io.h>
#include <util/delay.h>

#include "modeindicator.h"

uint8_t InitialiseModeIndicator()
{
  DDRD |= (1<<RED_LED) | (1<<GREEN_LED) | (1<<BLUE_LED);
  
  uint8_t i = 0;
  for (i = 0; i < 4; ++i)
  {
    PORTD ^= (1<<RED_LED) | (1<<GREEN_LED) | (1<<BLUE_LED);
    _delay_ms(250);
  }

  return 1;
}

