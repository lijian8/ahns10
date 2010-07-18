/**
 * \file   pwm.c
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
 * PWM Generating functions for ATMEGA328P Mode Control Unit
 */

#include "avrdefines.h"
#include <avr/io.h>

#include "pwm.h"

uint8_t InitialiseTimer0()
{
  // Register A 
  // COM0A = 10 for Non-inverting PWM
  // COM0B = 10
  // WGM0_210 = 001 for Normal PWM
  TCCR0A = (1 << COM0A1) | (0 << COM0A0) | 
           (1 << COM0B1) | (0 << COM0B0) | 
           (0 << WGM01) | (1 << WGM00);

  // Register B
  // No Clock Source Yet
  TCCR0B = (0 << WGM02); 

  // Outputs - OCR0A and OCR0B
  DDRD |= (1 << ESC1_PIN) | (1 << ESC2_PIN);

  return 1;
}

uint8_t InitialiseTimer1()
{   
  // Register A 
  // COM1A = 10 for Non-inverting PWM
  // COM1B = 10
  // WGM0_3210 = 0001 for 8bit Normal PWM
  TCCR1A = (1 << COM1A1) | (0 << COM1A0) | 
           (1 << COM1B1) | (0 << COM1B0) | 
           (0 << WGM11) | (1 << WGM10);

  // Register B
  // No Clock Source Yet
  TCCR1B = (0 << WGM13) | (0 << WGM12); 

  // Outputs - OCR1A and OCR1B
  DDRB |= (1 << ESC3_PIN) | (1 << ESC4_PIN);

  return 1;
}

uint8_t StopPWM()
{
  // Set ESC's to zero
  ESC1_COUNTER = 0;
  ESC2_COUNTER = 0;
  ESC3_COUNTER = 0;
  ESC4_COUNTER = 0;

  // Disable Clocks
  TCCR0B &= (0 << CS02) | (0 << CS01) | (0 << CS00);
  TCCR1B &= (0 << CS12) | (0 << CS11) | (0 << CS10);

  return 1;
}

uint8_t StartPWM()
{
  // CS0_210 = 011 for N = 64
  TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  
  ESC1_COUNTER = 0;
  ESC2_COUNTER = 0;
  ESC3_COUNTER = 0;
  ESC4_COUNTER = 0;

  return 1;
}

uint8_t SetPWM(uint32_t pulseHighTime)
{
  // Convert micro-seconds to timer compare
  
  return pulseHighTime / (2.0 * PWM_DT_US) ;
}
