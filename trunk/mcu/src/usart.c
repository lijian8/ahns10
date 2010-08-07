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
#include "MCUCommands.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "mode.h"
#include "pulsecapture.h"

volatile uint32_t apLastCommands;
volatile uint8_t newAPCommands;
volatile uint8_t txCommands;
volatile uint8_t txPeriodic;


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
  UCSR0C = (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

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
  // State of Machine
  static uint8_t rxState = HEADER_SYNC;
  uint8_t tempData = UDR0;
  
  // Temp Variables for SEND_MCU_COMMANDS
  static enum FlightModes tempMode = MANUAL_DEBUG;
  static uint8_t tempHeader = 255;
  static int8_t tempThrottle = 0, tempRoll = 0, tempPitch = 0, tempYaw = 0;

  switch (rxState)
  {
    case HEADER_SYNC: // wait for header sync char
      if (tempData == FRAME_CHAR)
      {
        rxState = HEADER_TYPE;
      }
      break;
    case HEADER_TYPE: // got first char, see if the header is here
      if ((tempData == GET_MCU_COMMANDS) || (tempData == GET_MCU_PERIODIC) || (tempData == SEND_MCU_COMMANDS))
      {
	tempHeader = tempData;
        rxState = HEADER_COMPLETE;
      }
      else
      {
        rxState = HEADER_SYNC;
      }
      break;
    case HEADER_COMPLETE:
      if (tempData == FRAME_CHAR) // header was valid
      {
	if (tempHeader == GET_MCU_COMMANDS)// Request sent for Commands
	{
          txCommands = 1;
	  rxState = HEADER_SYNC;
	}
	else if (tempHeader == GET_MCU_PERIODIC) // request for mode and engine data
	{
          txPeriodic = 1;
	  rxState = HEADER_SYNC;
	}
	else if (tempHeader == SEND_MCU_COMMANDS) // incoming parameters
	{
          rxState = RX_MODE;
	}
	else // unknown
	{
          rxState = HEADER_SYNC;
	}
      }
      else // header not valid
      {
	tempHeader = 255;
        rxState = HEADER_SYNC;
      }
      break;
    case RX_MODE:
      if ((tempData == MANUAL_DEBUG) || (tempData == AUGMENTED) || (tempData == AUTOPILOT))
      {
	tempMode = tempData;
        rxState = RX_THROTTLE;
      }
      else
      {
        rxState = HEADER_SYNC;
      }
      break;
    case RX_THROTTLE:
      tempThrottle = tempData;
      rxState = RX_ROLL;
      break;
    case RX_ROLL:
      tempRoll = tempData;
      rxState = RX_PITCH;
      break;
    case RX_PITCH:
      tempPitch = tempData;
      rxState = RX_YAW;
      break;
    case RX_YAW:
      tempYaw = tempData;
      rxState = RX_COMPLETE;
      break;
    case RX_COMPLETE:
      if (tempData == FRAME_CHAR)
      {
        newAPCommands = 1;
	apLastCommands = micro();

	apMode = tempMode;
        apThrottle = tempThrottle;
	apRoll = tempRoll;
	apPitch = tempPitch;
	apYaw = tempYaw;
      }
      rxState = HEADER_SYNC;
  }
}

inline void USARTtxCommands()
{
  // Frame
  USARTtxData(FRAME_CHAR);

  // Throttle
  USARTtxData(commandedThrottle);
  
  // Roll
  USARTtxData(commandedRoll);

  // Pitch
  USARTtxData(commandedPitch);

  // Yaw
  USARTtxData(commandedYaw);

  // End Frame
  USARTtxData(FRAME_CHAR);
  return;
}

inline void USARTtxPeriodic()
{
  uint16_t tempEngine = 0;

  USARTtxData(FRAME_CHAR);

  USARTtxData(flightMode);
  tempEngine = CounterToPWM(ESC1_COUNTER);
  USARTtxData((unsigned char) tempEngine >> 8); // MSB first
  USARTtxData((unsigned char) tempEngine);

  tempEngine = CounterToPWM(ESC2_COUNTER);
  USARTtxData((unsigned char) tempEngine >> 8); // MSB first
  USARTtxData((unsigned char) tempEngine);

  tempEngine = CounterToPWM(ESC3_COUNTER);
  USARTtxData((unsigned char) tempEngine >> 8); // MSB first
  USARTtxData((unsigned char) tempEngine);

  tempEngine = CounterToPWM(ESC4_COUNTER);
  USARTtxData((unsigned char) tempEngine >> 8); // MSB first
  USARTtxData((unsigned char) tempEngine);

  USARTtxData(FRAME_CHAR);
  return;
}
