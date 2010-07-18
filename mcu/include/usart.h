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

/**
 * @brief Output File for debugging
 */
extern FILE debugOut;

/**
 * @brief Initialise the USART for Tx and Rx
 */
extern uint8_t InitialiseUSART();

/**
 * @brif Transmit 8 bit Data on USART
 */
extern void USARTtxData(unsigned char txData);

/**
 * @brief Transmit char on USART for use with printf
 */
extern void USARTtxChar(char txChar, FILE *outStream);

#endif // USART_H
