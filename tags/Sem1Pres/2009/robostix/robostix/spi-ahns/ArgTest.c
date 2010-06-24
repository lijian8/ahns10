/****************************************************************************
*
*   Copyright (c) 2006 Dave Hylands     <dhylands@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
*   See README and COPYING for more details.
*
****************************************************************************/
/**
*
*   @file    ArgTest.c
*
*   @brief   Test program for testing the Args module.
*
*****************************************************************************/

// ---- Include Files -------------------------------------------------------

#include <stdio.h>
#include <avr/eeprom.h>

#include "Args.h"
#include "Config.h"
#include "Delay.h"
#include "Hardware.h"
#include "UART.h"

// ---- Public Variables ----------------------------------------------------

// ---- Private Constants and Types -----------------------------------------

// ---- Private Function Prototypes -----------------------------------------

// ---- Functions -----------------------------------------------------------

/***************************************************************************/
/**
*  Print out the arguments stored in EEPROM
*/

int main( void )
{
    uint8_t i;
    uint8_t numArgs;
    /* char    argStr[ 20 ]; */

    InitHardware();

   ASSR &= ~( 1 << AS0 );  // Make sure Port G LED pins are setup for I/O

    RED_LED_DDR     |= RED_LED_MASK;
    YELLOW_LED_DDR  |= YELLOW_LED_MASK;
    BLUE_LED_DDR    |= BLUE_LED_MASK;


    // The first handle opened for read goes to stdin, and the first handle
    // opened for write goes to stdout. So u0 is stdin, stdout, and stderr

#if defined( __AVR_LIBC_VERSION__ )
    fdevopen( UART0_PutCharStdio, UART0_GetCharStdio );
#else
    fdevopen( UART0_PutCharStdio, UART0_GetCharStdio, 0 );
#endif

    /* printf( "*****\n" );
     printf( "***** ArgTest program\n" );
     printf( "*****\n" ); */

    numArgs = NumArgs(); 

    LED_OFF( RED );
    LED_OFF( BLUE );
    LED_OFF( YELLOW );

    while ( 1 )
    {
        printf( "Number of arguments: %d\n", 3 );
        printf( "\n" );

        for ( i = 0; i < numArgs; i++ ) 
        {
            //GetArg( i, argStr, sizeof( argStr ));

            //printf( "arg[ %d ] = '%s'\n", i, argStr );
        }

        for ( i = 0; i < 5; i++ ) 
        {

	  LED_ON( RED );	    
	  putchar( 'a' );
	
	  

	  
	  LED_ON( BLUE );	    
	  putchar( 'b' );
	  
	  


	  
	  LED_ON( YELLOW );
	  putchar( 'c' );
	  
	  

	  
	  putchar( '\n' );

	  

        }
        putchar( '\n' );

	LED_OFF( RED );
	LED_OFF( BLUE );
	LED_OFF( YELLOW );
	
	  
	

    }

} // main

