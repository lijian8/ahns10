/**
 * @file   arduserial.c
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
 * Arduino serial library which implements:
 * - open and closing the serial connection with the arduino
 * - getting the compass heading from the arduino
 * ... others
 *
 */

#include "arduserial.h"

// struct to save the port settings
struct termios currentSerialPort, previousSerialPort;
// specified serial port
int arduSerialfd = 0;

/**
  * @brief Open the serial connection to the IMU
  */
int openArduSerial(char* serialPort, int baudRate)
{
  // open the specified serial port
  arduSerialfd = open(serialPort,O_RDWR | O_NOCTTY | O_NDELAY);
  // check if the serial port has been opened
  if (arduSerialfd == -1) 
  {
    // failed to open the serial port
    return 0; 
  } else
    {
      //fcntl(arduSerialfd, F_SETFL, 0);
    }
  // get serial port settings
  if(tcgetattr(arduSerialfd, &previousSerialPort)==-1)
  {
    // failed to read the serial port settings
    closeArduSerial();
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
  if((tcsetattr(arduSerialfd, TCSANOW, &currentSerialPort))==-1)
  {
    // failed to apply port settings
    closeArduSerial();
    return 0;
  }
  // succesfully opened the port
  return 1;
}

/**
  * @brief Close the serial connection to the arduino
  */
int closeArduSerial()
{  
  close(arduSerialfd);
  tcsetattr(arduSerialfd, TCSANOW, &previousSerialPort);
  return 1;
}

/**
  * @brief get the compass heading from the arduino
  */
int getCompassHeading(double *compassHeading)
{
  unsigned char sCmd[1] = {'C'};
  // send the command to the arduino
  if (!write(arduSerialfd,sCmd,1))
  {
    printf("Write failed\n");
    closeArduSerial();
    return 0;
  }
  usleep(DELAYRDWR);
  // read the compass heading returned
  unsigned char sResult[6];
  if(!read(arduSerialfd,sResult,5))
  {
    printf("Read failed\n");
    closeArduSerial();
    return 0;
  }
  // terminate with null character
  sResult[5] = 0x00;
  //printf("%s\n", sResult);
  // save to compass heading
  sscanf(sResult, "%lf", compassHeading);
  return 1;
}

