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

/** @name PWM Defines */
extern const double F_PWM;       /**< Pre-scalar of 64 */
extern const double PWM_DT_US;   /**< Time Resolution of generated PWM signals */
#define PWM_MAX_US 2000 /**< Maximum Bound on PWM */
#define PWM_MIN_US 700 /**< Minimum Bound on PWM */       

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
#define ESC1_MIN 1200

#define ESC2_MAX 2000
#define ESC2_MIN 1200

#define ESC3_MAX 2000
#define ESC3_MIN 1200

#define ESC4_MAX 2000
#define ESC4_MIN 1200

extern uint8_t escLimits[4][2];

/** @name PWM Timer Functions */
extern uint8_t InitialiseTimer0();
extern uint8_t InitialiseTimer1();

/** @name PWM Control Functions */
extern uint8_t StartPWM();
extern uint8_t StopPWM();

/** @name System Sec Timer */
extern volatile uint32_t systemSec;

/**
 * @brief Convert micro-seconds of high to timer 8 bit range
 */
#define PWMToCounter(pulseHighTime) ((pulseHighTime) / (2.0 * PWM_DT_US));

#endif // PWM_H
