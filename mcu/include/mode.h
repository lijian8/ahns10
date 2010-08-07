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

/** @brief Moving Average Function */
extern int16_t MovingAverage(int16_t* valueArray, uint8_t arrayLength);

/**
 * @brief Flight Mode
 */
extern volatile enum FlightModes flightMode;
extern volatile uint8_t failSafe;

/** 
 * @brief RC Command Inputs from Pulse Capture
 */
extern enum FlightModes rcMode; /**< RC Commanded Mode */
extern int8_t rcThrottle;       /**< RC Commanded Throttle */
extern int8_t rcRoll;          /**< RC Commanded Roll */
extern int8_t rcPitch;         /**< RC Commanded Pitch */
extern int8_t rcYaw;           /**< RC Commanded Yaw  */

/** 
 * @brief AP Command Inputs from USART Link
 */
extern volatile enum FlightModes apMode; /**< AP Commanded Mode */
extern volatile int8_t apThrottle;    /**< AP Commanded Throttle */
extern volatile int8_t apRoll;          /**< AP Commanded Roll */
extern volatile int8_t apPitch;         /**< AP Commanded Pitch */
extern volatile int8_t apYaw;           /**< AP Commanded Yaw  */

/**
 * @brief Commanded Values
 */
extern volatile int8_t commandedThrottle;
extern volatile int8_t commandedRoll;
extern volatile int8_t commandedPitch;
extern volatile int8_t commandedYaw;

/**
 * @brief Generate the low level esc commands
 * 
 * Involves Mixing the high level inputs to low level commands.
 * To limit motor failure or performance loss a moving average filter will
 * be applied to the current and previous inputs.
 */
extern void MixCommands();


/**
 * @brief Generate the High Level Commands
 * 
 * Directs if AP or RC Pulses should be used.
 */
extern void CombineCommands();
#endif // MODE_H
