/**
 * \file   usart.h
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
 * Declaration of the USART functions for the MCU
 */

#include "avrdefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#ifndef USART_H
#define USART_H

/** USART FC Baud Rate */
#define BAUD_RATE 57600
//#define BAUD_RATE 115200

/** Time of Last Received AP Command Packet */
extern volatile uint32_t apLastCommands;
 
/** New Autopilot Commands Received */
extern volatile uint8_t newAPCommands;

/** @name USART RX State */
enum {
	HEADER_SYNC,
	HEADER_TYPE,
	HEADER_COMPLETE,
	RX_MODE,
	RX_THROTTLE, 
	RX_ROLL,
	RX_PITCH,
	RX_YAW,
	RX_COMPLETE
};

/** @name USART TX Flags */
extern volatile uint8_t txCommands; /**< Flag if Commands need to be sent */
extern volatile uint8_t txRCCommands; /**< Flag if RC Commands need to be sent */
extern volatile uint8_t txPeriodic; /**< Flag if Engine and Mode need to be sent*/

/**
 * @brief Output File for debugging
 */
extern FILE debugOut;

/**
 * @brief Initialise the USART for Tx and Rx
 */
extern uint8_t InitialiseUSART();

/**
 * @brief Transmit 8 bit Data on USART
 */
extern void USARTtxData(volatile unsigned char txData);

/**
 * @brief Transmit char on USART for use with printf
 */
extern void USARTtxChar(char txChar, FILE *outStream);

/**
 * @brief Transmit MCU Commands to FC
 * The data to be sent is
 *   - Commanded Throttle
 *   - Commanded Roll
 *   - Commanded Pitch
 *   - Commanded Yaw
 */
extern void USARTtxCommands();

/**
 * @brief Transmit RC Commands to FC
 * The data to be sent is
 *   - RC Throttle
 *   - RC Roll
 *   - RC Pitch
 *   - RC Yaw
 */
extern void USARTtxRCCommands();

/**
 * @brief Transmit MCU Periodic Data to FC
 * The data to be sent is 
 *   - Flight Mode
 *   - Commanded Engine Pulse Widths in us
 */
extern void USARTtxPeriodic();

#endif // USART_H
