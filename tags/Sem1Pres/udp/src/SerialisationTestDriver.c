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

int main (int argc, char* argv[])
{
  unsigned char Buffer[2];

  printf("Little Endian Test Driver :\n");
    
  printf("Case 1: 16 bit to and from Network: \n");
  uint16_t original = 1;
  uint16_t test = 0;
  
  PackUInt16(Buffer,original);
  UnpackUInt16(Buffer,&test);

  if ( test == original )
  {
    printf ("Pass\n");
  }
  else 
  {
    printf("Fail: Original %d Final %d\n",original,test); 
  }
   {
  unsigned char Buffer[1];
  printf("Case 2: 8 bit to from Network: \n");
  uint8_t original = 10;
  uint8_t test = 0;
  
  PackUInt8(Buffer,original);
  UnpackUInt8(Buffer,&test);

  if ( test == original )
  {
    printf ("Pass\n");
  }
  else 
  {
    printf("Fail: Original %d Final %d\n",original,test); 
  }
 }

 {
  unsigned char Buffer[1];
  printf("Case 3: 64 bit to from Network: \n");
  uint64_t original = 155553;
  uint64_t test = 0;
  
  PackUInt64(Buffer,original);
  UnpackUInt64(Buffer,&test);

  if ( test == original )
  {
    printf ("Pass: Original %llu Final %llu\n",original,test);
  }
  else 
  {
    printf("Fail: Original %llu Final %llu\n",original,test); 
  }
 }

 
  

  return 0;
}
