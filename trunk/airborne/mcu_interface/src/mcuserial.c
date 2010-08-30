/**
 * @file   mcuserial.c
 * @author Tim Molloy
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
 * - Transmit Autopilot Mode and Commands
 * - Receive Flight Mode and Commanded Engine Pulse Widths
 * - Receive Flight Mode and High Level Commands
 */

#include "mcuserial.h"
#include "MCUCommands.h"

#include <sys/ioctl.h>

// struct to save the port settings
static struct termios currentSerialPort, previousSerialPort;
// specified serial port
static int fd = 0;

/**
  * @brief Close the serial connection to the IMU
  */
inline int mcuCloseSerial()
{  
  close(fd);
  tcsetattr(fd, TCSANOW, &previousSerialPort);
  return 1;
}

/**
  * @brief Open the serial connection to the IMU
  * @return 0 for fail, 1 for success
  */
inline int mcuOpenSerial(const char* serialPort, const int baudRate)
{
  // open the specified serial port
  fd = open(serialPort,O_RDWR | O_NOCTTY | O_NDELAY);
  
  // check if the serial port has been opened
  if (fd == -1) 
  {
    // failed to open the serial port
    return 0; 
  } 
  else
  {
    //fcntl(fd, F_SETFL, 0);
  }
  
  // get serial port settings
  if((tcgetattr(fd, &previousSerialPort) == -1) || (!isatty(fd)))
  {
    // failed to read the serial port settings or not serial port
    mcuCloseSerial();
    return 0;
  }
  // create a new memory structure for the port settings
  memset(&currentSerialPort, 0, sizeof(currentSerialPort));
  
  // set the baud rate, 8N1, enable the receiver, set local mode, no parity
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
    mcuCloseSerial();
    return 0;
  }
  // succesfully opened the port
  return 1;
}


/**
 * @brief Query MCU for Flight mode and Commanded Engine Data
 * @param flightMode Location of flightMode
 * @param commandedEngine Location of array of Commanded Engine Pulses
 * @return 1 for success, 0 for failure
 */
inline int getMCUPeriodic(uint8_t *flightMode, uint16_t *commandedEngine)
{
  const int bufferSize = 5;
  int returnValue = 0;
  unsigned char buffer[bufferSize];
  
  // Query MCU
  buffer[0] = FRAME_CHAR;
  buffer[1] = GET_MCU_PERIODIC;  
  buffer[2] = FRAME_CHAR;

  if (write(fd,buffer,3)) //write success
  {
    usleep(MCU_DELAYRDWR);
    if(!read(fd,buffer,sizeof(buffer)))  
    {
      fprintf(stderr,"MCU get periodic read failed\n");
    }
    else
    {
      *flightMode = buffer[0];
      commandedEngine[0] = CounterToPWM(buffer[1]);
      commandedEngine[1] = CounterToPWM(buffer[2]);
      commandedEngine[2] = CounterToPWM(buffer[3]);
      commandedEngine[3] = CounterToPWM(buffer[4]);
      returnValue = 1;
    }
  }
  return returnValue;
}


/**
 * @brief Query MCU for High Level Commands
 * @param commandedThrottle Location to store Commanded Throttle
 * @param commandedRoll Location to store Commanded Roll
 * @param commandedPitch Location to store Commanded Pitch
 * @param commandedYaw Location to store Commanded Yaw
 * @return 1 for success, 0 for failure
 */
inline int getMCUCommands(int8_t *commandedThrottle, int8_t *commandedRoll, int8_t *commandedPitch, int8_t *commandedYaw)
{
  const int bufferSize = 4;
  int returnValue = 0;
  int i = 0;
  unsigned char buffer[bufferSize];

  // Query MCU
  buffer[0] = FRAME_CHAR;
  buffer[1] = GET_MCU_COMMANDS;  
  buffer[2] = FRAME_CHAR;

  if (write(fd,buffer,3)) //write success
  {
    usleep(MCU_DELAYRDWR);
    if(!read(fd,buffer,sizeof(buffer))) 
    {
      fprintf(stderr,"MCU get commands read failed\n");
    }
    else
    {
	*commandedThrottle = buffer[0];
	*commandedRoll = buffer[1];
	*commandedPitch = buffer[2];
	*commandedYaw = buffer[3];
        returnValue = 1;
    }
  }
  return returnValue;
}

int sendMCUCommands(const volatile uint8_t *flightMode, const volatile int8_t *commandedThrottle, const volatile int8_t *commandedRoll, const volatile int8_t *commandedPitch, const volatile int8_t *commandedYaw)
{
  int returnValue = 0;
  unsigned char buffer[9];

  // Fill the buffer
  buffer[0] = FRAME_CHAR;
  buffer[1] = SEND_MCU_COMMANDS;
  buffer[2] = FRAME_CHAR;
  buffer[3] = *flightMode;
  buffer[4] = *commandedThrottle;
  buffer[5] = *commandedRoll;
  buffer[6] = *commandedPitch;
  buffer[7] = *commandedYaw;
  buffer[8] = FRAME_CHAR;
  
  if (write(fd,buffer,9)) // write success
  {
    returnValue = 1;
  }
  return returnValue;
}
