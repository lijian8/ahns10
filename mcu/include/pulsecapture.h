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
#define PC_DT_US (1e6*64.0/F_CPU)

/** @name Input Pins for Pulse Capture using Pin Change Interrupts */
#define NUM_CHANNELS 6
#define CHANNEL1 PC0
#define CHANNEL2 PC1
#define CHANNEL3 PC2
#define CHANNEL4 PC3
#define CHANNEL5 PC4
#define CHANNEL6 PC5

/** @name Input Channel Data Structure */
typedef struct
{
  uint8_t isHigh;               /**< Flag indicating detected channel status */
  uint8_t startTimerCount;      /**< Count of the Timer when channel last went high */
  uint8_t overflowCount;        /**< Number of times Timer has overflowed since channel went high*/
  uint32_t measuredPulseWidth;  /**< Previous Captured Pulse Width of the channel in US*/
} Channel;

/** @brief Array of Input Channels */
extern volatile Channel inputChannel[NUM_CHANNELS];

/** @brief Register of PINC Ports indicating those going high last PCINT */
extern volatile uint8_t risenPINC;

/** @brief Register of PINC Ports indicating those going low last PCINT */
extern volatile uint8_t fallenPINC;

/** @brief Timer Count at last PCINT */
extern volatile uint8_t countChanged;

/**
 * @ brief Initialise Port C and the required Pin Change Interrupts
 */
uint8_t InitialisePC();


/**
 * @brief Update Channel Information based on interrupt data
 * 
 * Called from outside the timer and pin capture interrupts in the main.
 */
void ProcessPC();

/** 
 * @brief Initialise Timer2 for Timing Pulse Width
 *
 * Runs at 1 MHz in 8 bit mode to enable 1us resolution in timing
 */
uint8_t InitialiseTimer2();

#endif // PULSECAPTURE_H
