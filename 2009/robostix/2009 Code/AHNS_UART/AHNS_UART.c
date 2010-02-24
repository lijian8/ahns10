/****************************************************************************
*
*   QUT - AHNS Project
*	Scott Osborne
*
****************************************************************************/

#include <avr/io.h>
#include <stdio.h>

#include "Hardware.h"
#include "Timer.h"
//#include "Timer.c"
//#include "UART.h"
//#include "UART.c"

//BAUD rate Calculation
//#define FOSC 16000000L// Clock Speed
//#define BAUD 115200
#define MYUBRR 8//FOSC/16/BAUD-1



uint8_t TxData8b;


#include <stdint.h>

#include <avr/io.h>


// Define baud rate

//#define UART1_BAUD 115200ul

//#define UART1_UBBR_VALUE ((F_CPU/(UART1_BAUD<<4))-1)


void UART1_vInit(void)

{
// Set baud rate
UBRR1H = (uint8_t)(MYUBRR>>8);
UBRR1L = (uint8_t)MYUBRR;
// Set frame format to 8 data bits, no parity, 1 stop bit
//UCSR1C = (0<<USBS1)|(1<<UCSZ1)|(1<<UCSZ0);
//UCSR1C = (0<<USBS1)|(3<<UCSZ1)|(0<<UPM10);
// Enable receiver and1 transmitter
UCSR1B = (1<<RXEN1)|(1<<TXEN1);
}

void UART0_vInit(void)

{
// Set baud rate
UBRR0H = (uint8_t)(MYUBRR>>8);
UBRR0L = (uint8_t)MYUBRR;
// Set frame format to 8 data bits, no parity, 1 stop bit
//UCSR1C = (0<<USBS1)|(1<<UCSZ1)|(1<<UCSZ0);
//UCSR1C = (0<<USBS1)|(3<<UCSZ1)|(0<<UPM10);
// Enable receiver and1 transmitter
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

void UART1_SendByte(uint8_t Data)
{
// Wait if a byte is being transmitted
/*while((UCSR1A&(1<<UDRE1)) == 0);
// Transmit data
UDR1 = Data;*/
while ( !( UCSR1A & (1<<UDRE1)) )
;
/* Put data into buffer, sends the data */
UDR1 = Data;
}

void UART0_SendByte(uint8_t Data)
{
// Wait if a byte is being transmitted
/*while((UCSR1A&(1<<UDRE1)) == 0);
// Transmit data
UDR1 = Data;*/
while ( !( UCSR0A & (1<<UDRE0)) )
;
/* Put data into buffer, sends the data */
UDR0 = Data;
}


int main(void)
{

int i;
//int TStamp;

UART0_vInit();
UART1_vInit();


while ( 1 )
{

    InitTimer();

    ASSR &= ~( 1 << AS0 );  // Make sure Port G LED pins are setup for I/O

    RED_LED_DDR     |= RED_LED_MASK;
    YELLOW_LED_DDR  |= YELLOW_LED_MASK;
    BLUE_LED_DDR    |= BLUE_LED_MASK;

	//TStamp++;

//Clear LED Status
	LED_OFF( RED );
    LED_OFF( BLUE );
	LED_OFF( YELLOW );


	LED_OFF( RED );
    LED_ON( BLUE );
	LED_OFF( YELLOW );

	//UART test output

	TxData8b = 0x41;
	UART1_SendByte(TxData8b);
	UART0_SendByte(TxData8b);
	TxData8b = 0x42;
	UART1_SendByte(TxData8b);
	UART0_SendByte(TxData8b);

	for ( i = 0; i < 50; i++ ) 
        {
        	WaitForTimer0Rollover();
		}

	LED_OFF( RED );
    LED_OFF( BLUE );
	LED_ON( YELLOW );

	TxData8b = 0x43;
	UART1_SendByte(TxData8b);
	TxData8b = 0x44;
	UART1_SendByte(TxData8b);

	for ( i = 0; i < 50; i++ ) 
        {
        	WaitForTimer0Rollover();
		}
		}
return 0;
}
