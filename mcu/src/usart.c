/**
 * \file   usart.c
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
 * Implementation of the USART functions for the MCU
 */

#include "avrdefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "mode.h"
#include "pulsecapture.h"

volatile uint16_t apLastSec;
volatile uint8_t newAPCommands;

FILE debugOut = FDEV_SETUP_STREAM(USARTtxChar, NULL,_FDEV_SETUP_WRITE);

uint8_t InitialiseUSART()
{
  uint8_t bRet = 1;
  uint16_t baudRateRegister = (uint16_t) (F_CPU/(16.0 * BAUD_RATE) - 1.0 + 0.5);

  // Control and Status Register A
  // Nothing to initialise

  // Control and Status Register B
  // Rx Interrupt Enabled
  // Rx and Tx Enabled
  // UCSZ0_210 = 011 for 8 bit data
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02) | (1 << RXCIE0);

  // Control and Status Register C
  // UMSEL0_10 = 00 for Asynchronous
  // UPM0_10 = 10 for Even Parity
  // USVS0 = 0 for 1 Stop Bit
  // UCSZ0_210 = 011 for 8 bit data
  UCSR0C = (1 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

  // Usart Baud Rate Register
  UBRR0H = (unsigned char) (baudRateRegister >> 8);
  UBRR0L =  (unsigned char) (baudRateRegister);

  return bRet;
}

void USARTtxChar(char txChar, FILE *outStream)
{
  if (txChar == '\n')
  {
    USARTtxChar('\r', outStream);
  }

  while (!BRS(UCSR0A,UDRE0)); // TODO wait with interrupt
  UDR0 = txChar;
  
  return;
}

void USARTtxData(unsigned char txChar)
{
  while (!BRS(UCSR0A,UDRE0)); // TODO wait with interrupt
  UDR0 = txChar;

  return;
}

/**
 * @brief USART Receive and Parse Data
 */
ISR(USART_RX_vect)
{
  // States of Machine
  enum {SYNC, MODE, THROTTLE, ROLL, PITCH, YAW, COMPLETE} rxState = SYNC;
  
  
  static int8_t tempMode = 0, tempThrottle = 0, tempRoll = 0, tempPitch = 0, tempYaw = 0;
  uint8_t tempData = UDR0;

  switch (rxState)
  {
    case SYNC:
      if (tempData == '#')
      {
        rxState = MODE;
      }
      break;
    case MODE:
      if ((tempData == MANUAL_DEBUG) || (tempData == AUGMENTED) || (tempData == AUTOPILOT))
      {
        rxState = THROTTLE;
	tempMode = tempData;
      }
      else
      {
        rxState = SYNC;
      }
      break;
    case THROTTLE:
      tempThrottle = tempData;
      rxState = ROLL;
      break;
    case ROLL:
      tempRoll = tempData;
      rxState = PITCH;
      break;
    case PITCH:
      tempPitch = tempData;
      rxState = YAW;
      break;
    case YAW:
      tempYaw = tempData;
      rxState = COMPLETE;
      break;
    case COMPLETE:      
      if (tempData == '#')
      {
        newAPCommands = 1;
	apLastSec = systemSec;

	apMode = tempMode;
        apThrottle = tempThrottle;
	apRoll = tempRoll;
	apPitch = tempPitch;
	apYaw = tempYaw;	
      }
      rxState = SYNC;
  }

}

