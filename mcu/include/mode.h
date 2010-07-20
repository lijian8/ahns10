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

/**
 * @brief Flight Mode
 */
extern volatile uint32_t flightMode;

enum {
       MANUAL_DEBUG,  /**< Manual Mode with no AP mixing */
       AUGMENTED,     /**< Manual Mode with additional AP mixing */
       AUTOPILOT      /**< AP Mode only*/
};

/** 
 * @brief RC Command Inputs from Pulse Capture
 */
extern volatile uint8_t RCcollective; /**< RC Commanded Collective */
extern volatile uint8_t RCroll;       /**< RC Commanded Roll */
extern volatile uint8_t RCpitch;      /**< RC Commanded Pitch */
extern volatile uint8_t RCyaw;        /**< RC Commanded Yaw  */

/** 
 * @brief AP Command Inputs from USART Link
 */
extern volatile uint8_t APcollective; /**< AP Commanded Collective */
extern volatile uint8_t AProll;       /**< AP Commanded Roll */
extern volatile uint8_t APpitch;      /**< AP Commanded Pitch */
extern volatile uint8_t APyaw;        /**< AP Commanded Yaw  */

/**
 * @brief Generate the low level esc commands from abstracted controls
 */
void mixCommands();

#endif // MODE_H
