


// ---- Include Files -------------------------------------------------------

#include <stdio.h>
#include <avr/eeprom.h>

#include "Timer.h"
#include "Config.h"
#include "Delay.h"
#include "Hardware.h"
#include "UART.h"
#include "spi.h"
#include "common.h"


int main( void )
{
  uint8_t i;
    /* uint8_t numArgs;*/
   // unsigned char rcv[ 8 ]={0x00}; 

  InitHardware();
  InitTimer();
  
  ASSR &= ~( 1 << AS0 );  // Make sure Port G LED pins are setup for I/O
  
  RED_LED_DDR     |= RED_LED_MASK;
  YELLOW_LED_DDR  |= YELLOW_LED_MASK;
  BLUE_LED_DDR    |= BLUE_LED_MASK;
  
  /* init SPI */
 SpiSlaveInit();
 LED_OFF( RED );
  
  // The first handle opened for read goes to stdin, and the first handle
  // opened for write goes to stdout. So u0 is stdin, stdout, and stderr
  
#if defined( __AVR_LIBC_VERSION__ )
  fdevopen( UART0_PutCharStdio, UART0_GetCharStdio );
#else
  fdevopen( UART0_PutCharStdio, UART0_GetCharStdio, 0 );
#endif

   unsigned char *bp;// = malloc(8);// = (unsigned char *)buffer;
 

    while ( 1 )
    {
       
       LED_OFF( RED );
      /* read SPI */
	SpiRead(bp, 8);	
        //printf("spi = %i \n",bp[0]);

      /* process SPI */

      /* send to the serial port */
      putchar('t');
      putchar('\n');	  

     LED_ON( RED );

      Delay100mSec(2);

      

       for ( i = 0; i < 100; i++ ) 
            {
             WaitForTimer0Rollover();
            }
 


    } 
	

    

} // main

