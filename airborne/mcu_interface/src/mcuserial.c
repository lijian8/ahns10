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
 * - Receive Commanded Engine Pulse Widths
 * - Receive High Level Commands and Flight Mode
 */

#include "mcuserial.h"

// struct to save the port settings
static struct termios currentSerialPort, previousSerialPort;
// specified serial port
static int fd = 0;

const int tempOffset = 80;

/**
  * @brief Open the serial connection to the IMU
  * @return 0 for fail, 1 for success
  */
inline int mcuOpenSerial(char* serialPort, int baudRate)
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
  * @brief Close the serial connection to the IMU
  */
inline int mcuCloseSerial()
{  
  close(fd);
  tcsetattr(fd, TCSANOW, &previousSerialPort);
  return 1;
}

/**
 * @brief Query MCU for Flight mode and Commanded Engine Data
 * @param flightMode Location of flightMode
 * @param commandedEngine Location of array of Commanded Engine Pulses
 * @return 0 for success, 1 for failure
 */
inline int getMCUPeriodic(uint8_t *flightMode, uint16_t *commandedEngine)
{
  int returnValue = 0;
  // Query MCU
  
  // Receive Flight Mode
  
  // Receive Commanded Engine Pulses
  
  return returnValue;
}


/**
 * @brief Query MCU for High Level Commands
 * @param commandedThrottle Location to store Commanded Throttle
 * @param commandedRoll Location to store Commanded Roll
 * @param commandedPitch Location to store Commanded Pitch
 * @param commandedYaw Location to store Commanded Yaw
 * @return 0 for success, 1 for failure
 */
inline int getMCUCommands(int8_t *commandedThrottle, int8_t *commandedRoll, int8_t *commandedPitch, int8_t *commandedYaw)
{
  int returnValue = 0;

  // Query MCU

  // Receive Throttle

  // Receive Roll

  // Receive Pitch

  // Receive Yaw

  return returnValue;
}

int sendMCUCommands(uint8_t *flightMode, int8_t *commandedThrottle, int8_t *commandedRoll, int8_t *commandedPitch, int8_t *commandedYaw)
{
  int returnValue = 0;
  unsigned char buffer[] = {'#','2','0','0','0','0','#'};
  int dataLength = sizeof(buffer);

  if (write(fd,buffer,dataLength)) // success
  {
    returnValue = 1;
  }
  return returnValue;
}
