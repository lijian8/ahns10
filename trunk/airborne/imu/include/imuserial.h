/**
 * @file   imuserial.h
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-05-20 01:32:15 +1000 (Thu, 20 May 2010) $
 * $Rev: 151 $
 * $Id: bfcameracomms.cpp 151 2010-05-19 15:32:15Z liamosullivan $
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
#define BAUD_RATE_DEFAULT B38400
// IMU baud rate
#define BAUD_RATE B115200	
// Delay between serial write and serial read (microseconds)
#define DELAYRDWR 20000

int openSerial(char* serialPort, int baudRate);
int closeSerial();
int setBaudRate(int baudRate);
int readCRBData(unsigned char reg);
int setCRBData(unsigned char reg, unsigned char value1, unsigned char value2);
int getImuSensorData(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd);
int startFreeRunningMode();
int stopFreeRunningMode();

#endif
