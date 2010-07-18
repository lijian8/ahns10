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
#define F_PWM ((double) F_CPU/(64.0*510.0))       /**< Pre-scalar of 64 */
#define PWM_DT_US ((double) (1e6 * 64.0 / F_CPU)) /**< Time Resolution of generated PWM signals */
#define PWM_MAX_US 2500 /**< Maximum Bound on PWM */
#define PWM_MIN_US 900  /**< Minimum Bound on PWM */       

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

/** @name PWM Timer Functions */
extern uint8_t InitialiseTimer0();
extern uint8_t InitialiseTimer1();

/** @name PWM Control Functions */
extern uint8_t StartPWM();
extern uint8_t StopPWM();
extern uint8_t SetPWM(uint32_t pulseHighTime);

#endif // PWM_H
