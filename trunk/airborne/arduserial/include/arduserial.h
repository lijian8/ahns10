/**
 * @file   arduserial.h
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
 * Arduino serial library header
 *
 */

#ifndef ARDUSERIAL_H
#define ARDUSERIAL_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

// Arduino baud rate
#define ARDU_BAUD_RATE B115200	
// Delay between serial write and serial read (microseconds)
#define ARDU_DELAYRDWR 0

int openArduSerial(char* serialPort, int baudRate);
int openArduSerialCan(char* serialPort, int baudRate);
int closeArduSerial();
int getCompassHeading(double *compassHeading);
int getBatteryVoltage(double *batteryVoltage);
int getAltitudeReading(double *altitudeReading);
int getArduinoData(double *compassHeading, double *batteryVoltage, double *altitudeReading);
int getArduinoDataCan(double *compassHeading, double *batteryVoltage, double *altitudeReading);

#endif
