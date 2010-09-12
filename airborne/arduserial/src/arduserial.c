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

  // no flow control
  currentSerialPort.c_cflag &= ~CRTSCTS;
  currentSerialPort.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

  currentSerialPort.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  currentSerialPort.c_oflag &= ~OPOST; // make raw

  // block until n bytes have been received
  currentSerialPort.c_cc[VMIN] = 0;
  currentSerialPort.c_cc[VTIME] = 0;
  // flush the serial port
  tcflush(arduSerialfd, TCIFLUSH);
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
    return 0;
  }
  usleep(ARDU_DELAYRDWR);
  // read the compass heading returned
  unsigned char sResult[6];
  if(!read(arduSerialfd,sResult,5))
  {
    printf("Read failed\n");
    return 0;
  }
  // terminate with null character
  sResult[5] = 0x00;
  // save to compass heading
  sscanf(sResult, "%lf", compassHeading);
  return 1;
}

/**
  * @brief get the battery voltage from the arduino
  */
int getBatteryVoltage(double *batteryVoltage)
{
  unsigned char sCmd[1] = {'V'};
  // send the command to the arduino
  if (!write(arduSerialfd,sCmd,1))
  {
    printf("Write failed\n");
    return 0;
  }
  usleep(ARDU_DELAYRDWR);
  // read the battery voltage
  unsigned char sResult[7];
  if(!read(arduSerialfd,sResult,6))
  {
    printf("Read failed\n");
    return 0;
  }
  // terminate with null character
  sResult[6] = 0x00;
  // save to battery voltage
  sscanf(sResult, "%lf", batteryVoltage);
  return 1;
}

/**
  * @brief get the altitude reading
  */
int getAltitudeReading(double *altitudeReading)
{
  unsigned char sCmd[1] = {'A'};
  // send the command to the arduino
  if (!write(arduSerialfd,sCmd,1))
  {
    printf("Write failed\n");
    return 0;
  }
  usleep(ARDU_DELAYRDWR);
  // read the altitude reading
  unsigned char sResult[6];
  if(!read(arduSerialfd,sResult,5))
  {
    printf("Read failed\n");
    return 0;
  }
  // terminate with null character
  sResult[5] = 0x00;
  // save to altitude reading
  sscanf(sResult, "%lf", altitudeReading);
  return 1;
}

/**
  * @brief get arduino data
  */
int getArduinoData(double *compassHeading, double *batteryVoltage, double *altitudeReading)
{
unsigned char sCmd[1] = {'O'};
  // send the command to the arduino
  if (!write(arduSerialfd,sCmd,1))
  {
    printf("Write failed\n");
    return 0;
  }
  usleep(ARDU_DELAYRDWR);
  // read the altitude reading
  unsigned char sResult[23];
  if(!read(arduSerialfd,sResult,22))
  {
    printf("Read failed\n");
    return 0;
  }
  // terminate with null character
  sResult[22] = 0x00;
  // save to altitude reading
  sscanf(sResult, "C%lf,V%lf,A%lf\n", compassHeading, batteryVoltage, altitudeReading);
  return 1;
}

int openArduSerialCan(char* serialPort, int baudRate)
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

  // no flow control
  currentSerialPort.c_cflag &= ~CRTSCTS;
  currentSerialPort.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

  currentSerialPort.c_lflag &= ICANON;

  // block until n bytes have been received
  currentSerialPort.c_cc[VMIN] = 22;
  currentSerialPort.c_cc[VTIME] = 0;
  // flush the serial port
  tcflush(arduSerialfd, TCIFLUSH);
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

int getArduinoDataCan(double *compassHeading, double *batteryVoltage, double *altitudeReading)
{
  usleep(ARDU_DELAYRDWR);
  // read the arduino data
  unsigned char sResult[255];
  int res = 0;
  res = read(arduSerialfd,sResult,255);
  // terminate with null character
  sResult[res] = 0x00;
  // save to altitude reading
  sscanf(sResult, "C%lf,V%lf,A%lf\n", compassHeading, batteryVoltage, altitudeReading);
  return 1;
}

