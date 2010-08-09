/**
 * @file   MCUCommands.h
 * @author Tim Molloy
 *
 * $Author: tlmolloy@gmail.com $
 * $Date: 2010-08-06 20:12:55 +1000 (Fri, 06 Aug 2010) $
 * $Rev: 282 $
 * $Id: mcuserial.c 282 2010-08-06 10:12:55Z tlmolloy@gmail.com $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * MCU Commands Header File
 * For use in the MCU and the FC software to enable shared Serial communication
 * - Defines Framing Characters
 * - MCU Commands
 */

#ifndef MCU_COMMANDS_H
#define MCU_COMMANDS_H

#define FRAME_CHAR '#'

enum {
	GET_MCU_COMMANDS = 0,
	GET_MCU_PERIODIC = 1,
	SEND_MCU_COMMANDS = 2
};

enum FlightModes {
       MANUAL_DEBUG,  /**< Manual Mode with no AP mixing */
       AUGMENTED,     /**< Manual Mode with additional AP mixing */
       AUTOPILOT,     /**< AP Mode only*/
       FAIL_SAFE      /**< MCU Failsafe Engaged*/ 
};

#endif // MCU_COMMANDS_H
