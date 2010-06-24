/**
 * @file   imuserial.h
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
 * Range serial library header
 *
 */

#ifndef RANGESERIAL_H
#define RANGESERIAL_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

// Range sensor baud rate (default)
#define BAUD_RATE_DEFAULT B9600	
// Delay between serial write and serial read (microseconds)
#define DELAYRDWR 12500

int openSerial(char* serialPort, int baudRate);
int closeSerial();
int getRangeData();

#endif
