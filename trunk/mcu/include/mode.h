/**
 * \file   mode.h
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-07-20 15:27:52 +1000 (Tue, 20 Jul 2010) $
 * $Rev: 248 $
 * $Id: usart.h 248 2010-07-20 05:27:52Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Declaration of MCU esc mixing modes
 */

#include "avrdefines.h"

#ifndef MODE_H
#define MODE_H

enum FlightModes {
       MANUAL_DEBUG,  /**< Manual Mode with no AP mixing */
       AUGMENTED,     /**< Manual Mode with additional AP mixing */
       AUTOPILOT      /**< AP Mode only*/
};


/** @brief Moving Average Function */
extern uint16_t MovingAverage(uint16_t* valueArray, uint8_t arrayLength);

/**
 * @brief Flight Mode
 */
extern volatile enum FlightModes flightMode;

/** 
 * @brief RC Command Inputs from Pulse Capture
 */
extern enum FlightModes rcMode; /**< RC Commanded Mode */
extern uint8_t rcThrottle;    /**< RC Commanded Throttle */
extern uint8_t rcRoll;          /**< RC Commanded Roll */
extern uint8_t rcPitch;         /**< RC Commanded Pitch */
extern uint8_t rcYaw;           /**< RC Commanded Yaw  */

/** 
 * @brief AP Command Inputs from USART Link
 */
extern volatile enum FlightModes apMode; /**< AP Commanded Mode */
extern volatile uint8_t apThrottle;    /**< AP Commanded Throttle */
extern volatile uint8_t apRoll;          /**< AP Commanded Roll */
extern volatile uint8_t apPitch;         /**< AP Commanded Pitch */
extern volatile uint8_t apYaw;           /**< AP Commanded Yaw  */

/**
 * @brief Generate the low level esc commands
 * 
 * Involves Mixing the high level inputs to low level commands.
 * To limit motor failure or performance loss a moving average filter will
 * be applied to the current and previous inputs.
 */
extern void MixCommands(volatile uint8_t* commandedThrottle, volatile uint8_t* commandedRoll, volatile uint8_t* commandedPitch, volatile uint8_t* commandedYaw);

#endif // MODE_H
