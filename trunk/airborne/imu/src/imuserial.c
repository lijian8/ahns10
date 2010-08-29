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
 * IMU serial library which implements:
 * - open and closing the serial connection with the IMU
 * - change the baudrate
 * - read or write CRB values
 * - request IMU data in one shot mode
 * - start or stop IMU data in free running mode
 *
 */

#include "imuserial.h"

// struct to save the port settings
struct termios currentSerialPort, previousSerialPort;
// specified serial port
int fd = 0;

// constants
// gyro scope offsets
const double mr1g = 0.0039;
const double mr2g = 0.0156;
// acceleration scope offsets
const double mr1a = 0.002031;
const double mr2a = 0.004062;
// gravity
const double g = 9.80665;
// temperature offset
const int tempOffset = 80;

/**
  * @brief Open the serial connection to the IMU
  */
int openIMUSerial(char* serialPort, int baudRate)
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
    closeIMUSerial();
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
    closeIMUSerial();
    return 0;
  }
  // succesfully opened the port
  return 1;
}

/**
  * @brief Close the serial connection to the IMU
  */
int closeIMUSerial()
{  
  close(fd);
  tcsetattr(fd, TCSANOW, &previousSerialPort);
  return 1;
}

/**
  * @brief Close the serial connection to the IMU
  */
int setBaudRate(int baudRate)
{ 
  // command to change baudrate
  unsigned char sCmd[6] = {'C','H','B','R',0x00,0x0D};
  switch (baudRate) {
    case 19200:
      sCmd[4] = 0;
    break;
    case 38400:
      sCmd[4] = 1;
    break;
    case 57600:
      sCmd[4] = 2;
    break;
    case 115200:
      sCmd[4] = 3;
    break;
    default:
      printf("Incorrect baudrate!\n");
      return 0;
    break;
  }
  // send the request to change the baud rate
  if (!write(fd,sCmd,6))
  {
    printf("Write failed\n");
    closeIMUSerial();
    return 0;
  }
  usleep(IMU_DELAYRDWR);
  // read the result
  unsigned char sResult[6];
  if(!read(fd,sResult,5))
  {
    printf("Read failed\n");
    closeIMUSerial();
    return 0;
  }
  // terminate with null character
  sResult[6] = 0x00;
  printf("%s", sResult);
  return 1;
}

/**
  * @brief Read the requested CRB register value
  */
int readCRBData(unsigned char reg)
{
  unsigned char sCmd[6] = {'R','C','R','B',reg,0x0D};
  // send the request to read the CRB reg value
  if (!write(fd,sCmd,6))
  {
    printf("Write failed\n");
    closeIMUSerial();
    return 0;
  }
  usleep(IMU_DELAYRDWR);
  // read the reg value returned
  unsigned char sResult[5];
  if(!read(fd,sResult,4))
  {
    printf("Read failed\n");
    closeIMUSerial();
    return 0;
  }
  // terminate with null character
  sResult[5] = 0x00;
  printf("%s", sResult);
  return 1;
}

/**
  * @brief Set the requested CRB register value
  */
int setCRBData(unsigned char reg, unsigned char value1, unsigned char value2)
{
  unsigned char sCmd[8] = {'W','C','R','B',reg,value1,value2,0x0D};
  // send the request to read the CRB reg value
  if (!write(fd,sCmd,8))
  {
    printf("Write failed\n");
    closeIMUSerial();
    return 0;
  }
  usleep(IMU_DELAYRDWR);
  // read the reg value returned
  unsigned char sResult[6];
  if(!read(fd,sResult,5))
  {
    printf("Read failed\n");
    closeIMUSerial();
    return 0;
  }
  // terminate with null character
  sResult[6] = 0x00;
  printf("%s", sResult);
  return 1;
}

/**
  * @brief Request and receive IMU data (one shot mode)
  */
int getImuSensorData(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd)
{
  unsigned char sCmd[7] = {'S','T','A','O','S','M',0x0D};

  if(!write(fd,sCmd,7))
  {
    printf("Write failed\n");
    closeIMUSerial();
    return 0;
  }

  unsigned char sResult[50];
  usleep(IMU_DELAYRDWR);
  if(!read(fd,sResult,49))
  {
    printf("Read failed\n");
    closeIMUSerial();
    return 0;
  }
  // calculate the CRC value
  unsigned char crcResult = 0;
  int i = 0;
  for(i=0; i<45;i++)
  {
    crcResult = crcResult + sResult[i];
  }
  crcResult = ~crcResult;

  short int rateX = 0;
  short int rateY = 0;
  short int rateZ = 0;
  short int accX = 0;
  short int accY = 0;
  short int accZ = 0;
  unsigned short int temp=0;
  unsigned char statusL=0;
  unsigned char statusH=0;
  unsigned char crc=0;

  //printf("crc given: %hx\n",crcResult);
  // terminate with null character
  sResult[50] = 0x00;
  //printf("%s",sResult);

  sscanf(sResult, "%hx,%hx,%hx,%hx,%hx,%hx,%hx,%hhx,%hhx,%hhx", &rateX, &rateY, &rateZ, &accX, &accY, &accZ, &temp, &statusL, &statusH, &crc);

  // calculate the rates
  *rateXd = rateX*mr2g;
  *rateYd = rateY*mr2g;
  *rateZd = rateZ*mr2g;
  // calculate the acceleration
  *accXd = accX*mr2a;
  *accYd = accY*mr2a;
  *accZd = accZ*mr2a;
  // calculate the temperature
  temp = temp-tempOffset;

  return 1;
}

/**
  * @brief Start free running mode
  */
int startFreeRunningMode()
{
  unsigned char sCmd[7] = {'S','T','A','F','R','M',0x0D};

  if(!write(fd,sCmd,7))
  {
    printf("Write failed\n");
    closeIMUSerial();
    return 0;
  }
  return 1;
}

/**
  * @brief Stop free runnung mode
  */
int stopFreeRunningMode()
{
  unsigned char sCmd[7] = {'S','T','P','F','R','M',0x0D};

  if(!write(fd,sCmd,7))
  {
    printf("Write failed\n");
    closeIMUSerial();
    return 0;
  }
  usleep(IMU_DELAYRDWR);
  // read the result
  unsigned char sResult[6];
  if(!read(fd,sResult,5))
  {
    printf("Read failed\n");
    closeIMUSerial();
    return 0;
  }
  // terminate with null character
  sResult[6] = 0x00;
  printf("%s", sResult);
  return 1;
}
