/**
 * @file   SerialisationTestDriver.c
 * @author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-06-10 23:59:05 +1000 (Thu, 10 Jun 2010) $
 * $Rev: 164 $
 * $Id: bfcameracomms.h 164 2010-06-10 13:59:05Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Test driver for Little Endian network to host and host to network functions
 */

#include <stdio.h>
#include <stdint.h>
#include <primitive_serialisation.h>
#include <netinet/in.h>
#include <sys/time.h>

int main (int argc, char* argv[])
{
  printf("Size of Timeval: %u\n",sizeof(struct timeval));
  printf("Little Endian Test Driver :\n");
  printf("Case 1: Time Test\n");
  
  struct timeval timeOriginal;
  struct timeval timeFinal;
  gettimeofday(&timeOriginal, 0);

  // Pack Time stamp but keep track of started
  char buffer[8];
  char* msgTimePointer = buffer;

  // 32-bit Transmission of Time
  PackInt32((unsigned char*) buffer,(int32_t) timeOriginal.tv_sec);  
  PackInt32((unsigned char*) &buffer[4],(int32_t) timeOriginal.tv_usec);

  // 32 bit Rx
  int32_t tv_sec, tv_usec;
  msgTimePointer += UnpackInt32((unsigned char*) msgTimePointer, &tv_sec);
  msgTimePointer += UnpackInt32((unsigned char*) msgTimePointer, &tv_usec);
  timeFinal.tv_sec = (int64_t) tv_sec;
  timeFinal.tv_usec = (int64_t) tv_usec;

  /*timeFinal.tv_sec = (int64_t) ntohl(*(int32_t*) msgTimePointer);
  msgTimePointer += 4;
  timeFinal.tv_usec = (int64_t) ntohl(*(int32_t*) msgTimePointer);
  msgTimePointer += 4;
  */


  printf("timeOriginal: %i %i \n", timeOriginal.tv_sec, timeOriginal.tv_usec);
  printf("timeFinal: %i %i \n", timeFinal.tv_sec, timeFinal.tv_usec);


  printf("***Case 2: ntohl Test of PackUInt32\n");
  

  // Pack original
  srand(time(NULL));
  uint32_t original = rand() % 100;
  uint32_t final = 0;
  msgTimePointer = buffer;

  // 32-bit Transmission of Time
  PackUInt32((unsigned char*) buffer,original);  

  // 32 bit Rx
  //msgTimePointer += UnpackUInt32((unsigned char*) msgTimePointer, &final);

  final = (uint32_t) ntohl(*(uint32_t*) msgTimePointer);


  printf("Original: %i \n", original);
  printf("Final: %i \n", final);

  printf("Case 3: PackUInt16");
 
  // Pack original
  srand(time(NULL));
  uint16_t sOriginal = rand() % 100;
  uint16_t sFinal = 0;
  msgTimePointer = buffer;

  // TX
  PackUInt16((unsigned char*) buffer,sOriginal);

  // RX 
//  msgTimePointer += UnpackUInt16((unsigned char*) msgTimePointer, &sFinal);

  sFinal = (uint16_t) ntohs(*(uint16_t*) msgTimePointer);


  printf("Original: %i \n", sOriginal);
  printf("Final: %i \n", sFinal);

  return 0;
}
