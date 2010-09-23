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
#define MCU_DELAYRDWR 1000

int mcuOpenSerial(const char* serialPort, const int baudRate);
int mcuCloseSerial();
int getMCUPeriodic(uint8_t* flightMode, uint16_t* commandedEngine);
int getMCUCommands(int8_t *commandedThrottle, int8_t *commandedRoll, int8_t *commandedPitch, int8_t *commandedYaw);
int sendMCUCommands(const volatile uint8_t *flightMode, const volatile int8_t *commandedThrottle, const volatile int8_t *commandedRoll, const volatile int8_t *commandedPitch, const volatile int8_t *commandedYaw);
int getRCCommands(int8_t *rcThrottle, int8_t *rcRoll, int8_t *rcPitch, int8_t *rcYaw);

#endif
