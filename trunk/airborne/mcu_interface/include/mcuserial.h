/**
 * @file   mcuserial.h
 * @author Liam O'Sullivan
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * MCU serial library header
 * - Open and close serial to MCU
 * - Receive Commanded Engine Pulse Widths
 * - Receive High Level Commands and Flight Mode
 */

#ifndef MCUSERIAL_H
#define MCUSERIAL_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdint.h>

/** MCU baud rate */
#define MCU_BAUD_RATE B57600

/** Delay between serial write and serial read (microseconds) */
#define MCU_DELAYRDWR 20000

int mcuOpenSerial(char* serialPort, int baudRate);
int mcuCloseSerial();
int getMCUPeriodic(uint8_t* flightMode, uint16_t* commandedEngine);
int getMCUCommands(int8_t *commandedThrottle, int8_t *commandedRoll, int8_t *commandedPitch, int8_t *commandedYaw);
int sendMCUCommands(uint8_t *flightMode, int8_t *commandedThrottle, int8_t *commandedRoll, int8_t *commandedPitch, int8_t *commandedYaw);

#endif
