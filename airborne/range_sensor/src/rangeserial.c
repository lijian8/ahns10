/**
 * @file   imuserial.c
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
 * Range serial library which implements:
 * - open and closing the serial connection with the range sensor
 * - read the raw sensor data and return a distance value
 *
 */

#include "rangeserial.h"

// struct to save the port settings
struct termios currentSerialPort, previousSerialPort;
// specified serial port
int fd = 0;

/**
  * @brief Open the serial connection to the Range sensor
  */
int openSerial(char* serialPort, int baudRate)
{

  // open the specified serial port
  fd = open(serialPort,O_RDWR | O_NOCTTY | O_NDELAY);
  // check if the serial port has been opened
  if (fd == -1) 
  {
    // failed to open the serial port
    return 0; 
  } else
    {
      //fcntl(fd, F_SETFL, 0);
    }
  // get serial port settings
  if(tcgetattr(fd, &previousSerialPort)==-1)
  {
    // failed to read the serial port settings
    closeSerial();
    return 0;
  }
  // create a new memory structure for the port settings
  memset(&currentSerialPort, 0, sizeof(currentSerialPort));
  // set the baud rate, 8N1, enable the receiver, set local mode
  currentSerialPort.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
  // ignore parity errors
  currentSerialPort.c_iflag = IGNPAR;
  currentSerialPort.c_oflag = 0;
  currentSerialPort.c_lflag = 0;
  // block until n bytes have been received
  currentSerialPort.c_cc[VMIN] = 0;
  currentSerialPort.c_cc[VTIME] = 0;
  // apply the settings to the port
  if((tcsetattr(fd, TCSANOW, &currentSerialPort))==-1)
  {
    // failed to apply port settings
    closeSerial();
    return 0;
  }
  // succesfully opened the port
  return 1;
}

/**
  * @brief Close the serial connection to the Range sensor
  */
int closeSerial()
{  
  close(fd);
  tcsetattr(fd, TCSANOW, &previousSerialPort);
  return 1;
}

/**
  * @brief get the Range Data
  */

int getRangeData(double *zRange)
{
  short int rawZ = 0;
  short int readCheck = 0;
  unsigned char sResult[6];
  unsigned char sRange[4];
  short int i = 0;
  //usleep(DELAYRDWR);
  while(!readCheck)
  {
    usleep(DELAYRDWR);
    read(fd,sResult,1);
    if (sResult[0] == 0x0D)
    {
      readCheck = 1;
    }
  }
  usleep(DELAYRDWR);
  if(!read(fd,sResult,5))
  {
    printf("Read failed\n");
    closeSerial();
    return 0;
  }
  sResult[5] = 0x00;
  for(i=0; i<3; i++)
  {
    sRange[i] = sResult[i+1];
  }
  sRange[3] = 0x00;
  rawZ = atoi(sRange);
  *zRange = rawZ*2.54;
  return 1;
}
