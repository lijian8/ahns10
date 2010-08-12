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

#include <sys/ioctl.h>

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
  currentSerialPort.c_cflag = baudRate | CS8 | CLOCAL | CREAD | CSTOPB;
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
  const int bufferSize = 7;
  int bytesReceived = 0, packetEnd = 6, packetStart = 0;
  int returnValue = 0;
  int i = 0;
  unsigned char buffer[bufferSize];

  // Query MCU
  buffer[0] = FRAME_CHAR;
  buffer[1] = GET_MCU_PERIODIC;  
  buffer[2] = FRAME_CHAR;

  if (write(fd,buffer,3)) //write success
  {
    // Read Periodic
    /*do
    {
      ioctl(fd, FIONREAD, &packetStart);
    } while(packetStart < sizeof(buffer));
   */
    //usleep(500);
    bytesReceived = read(fd,buffer,sizeof(buffer));  
    if(!bytesReceived)
    {
      fprintf(stderr,"MCU get periodic read failed\n");
    }
    else
    {
      // Find the frame chars in the buffer
      /*for (i = bytesReceived - 1; i >= 0; --i)
      {
        if ((buffer[i] == FRAME_CHAR) && (packetEnd == 0))
        {
          packetEnd = i;
        }
        else if (buffer[i] == FRAME_CHAR)
        {
          packetStart = i;
        }
      }*/
      if (buffer[packetStart] == buffer[packetEnd])
      {
	*flightMode = buffer[packetStart + 1];
        commandedEngine[0] = CounterToPWM(buffer[packetStart + 2]);
        commandedEngine[1] = CounterToPWM(buffer[packetStart + 3]);
        commandedEngine[2] = CounterToPWM(buffer[packetStart + 4]);
        commandedEngine[3] = CounterToPWM(buffer[packetStart + 5]);
        returnValue = 1;
      }
      else
      {
        fprintf(stderr,"MCU periodic buffer failed\n%i,%i\n",buffer[0],buffer[6]);
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
  const int bufferSize = 7;
  int bytesReceived = 0, packetEnd = 0, packetStart = 0;
  int returnValue = 0;
  int i = 0;
  unsigned char buffer[bufferSize];

  // Query MCU
  buffer[0] = FRAME_CHAR;
  buffer[1] = GET_MCU_COMMANDS;  
  buffer[2] = FRAME_CHAR;

  if (write(fd,buffer,3)) //write success
  {
    // Read Periodic
    /*do
    {
      ioctl(fd, FIONREAD, &packetStart);
    } while(packetStart < sizeof(buffer));
   */
    usleep(MCU_DELAYRDWR);
    bytesReceived = read(fd,buffer,sizeof(buffer));  
    if(!bytesReceived)
    {
      fprintf(stderr,"MCU get commands read failed\n");
    }
    else
    {
      // Find the frame chars in the buffer
      for (i = bytesReceived - 1; i >= 0; --i)
      {
        if ((buffer[i] == FRAME_CHAR) && (packetEnd == 0))
        {
          packetEnd = i;
        }
        else if (buffer[i] == FRAME_CHAR)
        {
          packetStart = i;
        }
      }
      if (buffer[packetStart] == buffer[packetEnd])
      {
	*commandedThrottle = buffer[packetStart + 1];
	*commandedRoll = buffer[packetStart + 2];
	*commandedPitch = buffer[packetStart + 3];
	*commandedYaw = buffer[packetStart + 4];
        returnValue = 1;
      }
      else
      {
        fprintf(stderr,"MCU get commands buffer failed\n%i,%i\n",buffer[0],buffer[6]);
      }
    }
  }
  return returnValue;
}

int sendMCUCommands(const uint8_t *flightMode, const int8_t *commandedThrottle, const int8_t *commandedRoll, const int8_t *commandedPitch, const int8_t *commandedYaw)
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
