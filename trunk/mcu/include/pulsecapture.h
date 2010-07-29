/**
 * \file   pulsecapture.h
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
 * Declarations for RC Pulse Capture of the MCU
 */

#include "avrdefines.h"
#include "pwm.h"

#include <avr/io.h>

#ifndef PULSECAPTURE_H
#define PULSECAPTURE_H

/** @name Pulse Capture Resolution in micro-seconds*/
extern const uint8_t PC_DT_US;

/** @name Input Pins for Pulse Capture using Pin Change Interrupts */
#define NUM_CHANNELS 6
#define CHANNEL1 PC0
#define CHANNEL2 PC1
#define CHANNEL3 PC2
#define CHANNEL4 PC3
#define CHANNEL5 PC4
#define CHANNEL6 PC5

/** @name RC Input Pulse Widths */
#define PC_PWM_MAX 2000
#define PC_PWM_MIN 1000
#define PULSE_TOLERANCE 400

/** @name Input Channel Data Structure */
typedef struct
{
  uint8_t isHigh;               /**< Flag indicating detected channel status */
  uint32_t startTime;            /**< Counter Value at Start Time */
  volatile uint16_t measuredPulseWidth;  /**< Previous Captured Pulse Width of the channel in US*/
} Channel;

/** @brief Array of Input Channels */
extern volatile Channel inputChannel[NUM_CHANNELS];

/** @name Flag for new RC pulses */
extern volatile uint8_t newRC;
extern volatile uint8_t failSafe;

/**
 * @ brief Initialise Port C and the required Pin Change Interrupts
 */
extern uint8_t InitialisePC();

/**
 * @brief Update the RC data values and check for failsafe conditions
 * 
 * Called after processing to check for receiver failsafe.
 * Converts the captured PWM widths to the counter values for use in global variables.
 */ 
extern void UpdateRC();

/** 
 * @brief Initialise Timer2 for Timing Pulse Width
 *
 * Runs at 125 kHz in 8 bit mode to enable 8us resolution in timing
 */
extern uint8_t InitialiseTimer2();

/** 
 * @brief Calculate Timer 2 System Time to resolution of 8us
 * Will overflow after 71 minutes due to 32bit return
 * @return System Time in micro-seconds since start up
 */
extern uint32_t micro();

#endif // PULSECAPTURE_H
