/**
 * \file   pwm.h
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
 * Header for PWM for ATMEGA328P Mode Control Unit
 */

#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

/** @name ESC Output Compare Counters */
#define ESC1_COUNTER OCR0A
#define ESC2_COUNTER OCR0B
#define ESC3_COUNTER OCR1A
#define ESC4_COUNTER OCR1B

/** @name ESC Output Pins*/
#define ESC1_PIN PD6
#define ESC2_PIN PD5
#define ESC3_PIN PB1
#define ESC4_PIN PB2

/** @name Maximum and Minimum ESC PWM Values*/ 
#define ESC1_MAX 2000
#define ESC1_MIN 1000

#define ESC2_MAX 2000
#define ESC2_MIN 1000

#define ESC3_MAX 2000
#define ESC3_MIN 1000

#define ESC4_MAX 2000
#define ESC4_MIN 1000

#define ESC_MIN 0
#define ESC_MAX 1

/** 
 * @name ESC Limits Array
 * First Column is min values
 * Second Column is max values 
 */
extern uint8_t escLimits[4][2];

/** @name PWM Timer Functions */
extern uint8_t InitialiseTimer0();
extern uint8_t InitialiseTimer1();

/** @name PWM Control Functions */
extern uint8_t StartPWM();
extern uint8_t StopPWM();

/** @name System Sec Timer */
extern volatile uint32_t systemSec;

#endif // PWM_H
