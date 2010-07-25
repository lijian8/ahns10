/**
 * \file   modeindicator.c
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
 * Implementation of Mode Indicator functions
 */

#include "avrdefines.h"

#include <avr/io.h>
#include <util/delay.h>

#include "modeindicator.h"

uint8_t InitialiseModeIndicator()
{
  DDRD |= (1 << RED_LED) | (1 << GREEN_LED) | (1 << BLUE_LED);
  
  uint8_t i = 0;
  for (i = 0; i < 4; ++i)
  {
    PORTD ^= (1 << RED_LED) | (1 << GREEN_LED) | (1 << BLUE_LED);
    _delay_ms(250);
  }

  return 1;
}

inline void ToggleRed(uint8_t mode)
{
  switch (mode)
  {
    case ON:
      PORTD |= SBR(RED_LED);
      break;
    case OFF:
      PORTD &= CBR(RED_LED);
      break;
    case TOGGLE:
      PORTD ^= (1 << RED_LED);
  }
}

inline void ToggleGreen(uint8_t mode)
{
  switch (mode)
  {
    case ON:
      PORTD |= SBR(GREEN_LED);
      break;
    case OFF:
      PORTD &= CBR(GREEN_LED);
      break;
    case TOGGLE:
      PORTD ^= (1 << GREEN_LED);
  }
}

inline void ToggleBlue(uint8_t mode)
{
  switch (mode)
  {
    case ON:
      PORTD |= SBR(BLUE_LED);
      break;
    case OFF:
      PORTD &= CBR(BLUE_LED);
      break;
    case TOGGLE:
      PORTD ^= (1 << BLUE_LED);
  }
}

