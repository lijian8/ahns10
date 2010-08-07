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
#include "MCUCommands.h"

// struct to save the port settings
static struct termios currentSerialPort, previousSerialPort;
// specified serial port
static int fd = 0;

const int tempOffset = 80;

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
  int returnValue = 0;
  unsigned char buffer[11];

  // Query MCU
  buffer[0] = FRAME_CHAR;
  buffer[1] = GET_MCU_PERIODIC;  
  buffer[2] = FRAME_CHAR;

  if (write(fd,buffer,3)) //write success
  {
    // Read Periodic
    usleep(MCU_DELAYRDWR);
    if(!read(fd,buffer,11)) // read fail
    {
      printf("Read failed\n");
      mcuCloseSerial();
    }
    else
    {
      if ((buffer[0] == buffer[10]) && (buffer[0] == FRAME_CHAR)) // read success
      {
	*flightMode = buffer[1];
        commandedEngine[0] = (uint16_t) (buffer[2] << 8) | (uint16_t) buffer[3];
        commandedEngine[1] = (uint16_t) (buffer[4] << 8) | (uint16_t) buffer[5];
        commandedEngine[2] = (uint16_t) (buffer[6] << 8) | (uint16_t) buffer[7];
        commandedEngine[3] = (uint16_t) (buffer[8] << 8) | (uint16_t) buffer[9];
        returnValue = 1;
      }
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
  int returnValue = 0;
  unsigned char buffer[6];

  // Query MCU
  buffer[0] = FRAME_CHAR;
  buffer[1] = GET_MCU_COMMANDS;  
  buffer[2] = FRAME_CHAR;

  if (write(fd,buffer,3)) //write success
  {
    usleep(MCU_DELAYRDWR);
    if(!read(fd,buffer,6)) // read fail
    {
      printf("Read failed\n");
      mcuCloseSerial();
    }
    else
    {
      if ((buffer[0] == buffer[5]) && (buffer[0] == FRAME_CHAR)) // read success
      {
        *commandedThrottle = buffer[1];
        *commandedRoll = buffer[2];
        *commandedPitch = buffer[3];
        *commandedYaw = buffer[4];
        returnValue = 1;
      }
    }
  }

  return returnValue;
}

int sendMCUCommands(const uint8_t *flightMode, const int8_t *commandedThrottle, const int8_t *commandedRoll, const int8_t *commandedPitch, const int8_t *commandedYaw)
{
  int returnValue = 0;
  unsigned char buffer[9];

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
