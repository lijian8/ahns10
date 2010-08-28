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
 * IMU serial library header
 *
 */

#ifndef IMUSERIAL_H
#define IMUSERIAL_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

// IMU baud rate (default)
#define IMU_BAUD_RATE_DEFAULT B38400
// IMU baud rate
#define IMU_BAUD_RATE B115200
// Delay between serial write and serial read (microseconds)
#define IMU_DELAYRDWR 20000

int openIMUSerial(char* serialPort, int baudRate);
int closeIMUSerial();
int setIMUBaudRate(int baudRate);
int readCRBData(unsigned char reg);
int setCRBData(unsigned char reg, unsigned char value1, unsigned char value2);
int getImuSensorData(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd);
int startFreeRunningMode();
int stopFreeRunningMode();

#endif
