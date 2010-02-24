/**
 * @file
 * @author Alex Wainwright, Owen Plagens
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-09-02 20:15:37 +1000 (Wed, 02 Sep 2009) $
 * $Rev: 526 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Device: ATmega8
 * RS232 input to ppm output
 * Takes serial commands from the autopilot, and converts them to a PPM signal for the buddy box port in the transmitter.
 */

//#define DEBUG

#include <avr/io.h>      // avr header file for IO ports
#include <avr/interrupt.h>
#include <stdio.h>

#define FOSC 8000000							/* System clock frequency */
#define BAUD 19200							/* Serial Baud Rate */
#define PRESCALER 1
#define SYS_UBRR FOSC/16/BAUD-1			/* USART Baud Rate Register Value */

#define WAIT_FOR_PACKET_START 1			// State flags
#define WAIT_FOR_PACKET_END 2
#define WAIT_FOR_SYNC1 11
#define WAIT_FOR_SYNC2 12
#define WAIT_FOR_SYNC0 13
#define WAIT_FOR_SYNC3 14
#define BUFFER_SIZE 7 

#define PPM_FACTOR 3.9215686
#define PPM_OFFSET 600

#define SYNC_BYTE '#' 

#define SETBIT(x) (1<<(x))					/* Set bit 'x' in register */
#define CLRBIT(x) ~(1<<(x))					/* Clear bit 'x' in register */	

/* Type definitions */
typedef unsigned char u8_bit;				/* Define 8 bit data type */
typedef unsigned int u16_bit;				/* Define 16 bit data type */
typedef unsigned long u32_bit;	 			/* Define 32 bit data type */
#define false 0
#define true 1

/* Global Strings */
u8_bit szInitialised[] = "..AVR INITIALISED..";	/* Initialised Lable */
u8_bit szResendData[] = "$ResendData";			/* Request for retranmission of servo data */
u8_bit szErrorMsg[] = "$ERROR";					/* Error msg generated due to unknown packet error */

u16_bit channels[7];	// channels for PPM output
u8_bit szSerBuffer[11];

/**
 * Scales from channel values to microseconds
 */
inline u16_bit ppmscale(scale_in) 
{
	return (PPM_FACTOR*scale_in + PPM_OFFSET); //scales 0-255 to 600-1600 linearly 
}

/** 
 * Sets the output compare register OCR1A given the number of microseconds to wait.
 * @param time The time to wait in microseconds
 */
inline void ppmtime(u16_bit time) 
{
	unsigned char sreg = SREG;
	cli();
	OCR1A += (time)*(FOSC/1000000)/PRESCALER;
	sei();
	SREG = sreg;
}

void PPM_Init();
void USART_Init();
void Tx_Byte(u8_bit Tx_Data);
void Tx_String(u8_bit *Packet);
void GetSerPacket(u8_bit szByteRx);
void delay_ms(u8_bit delay);

/// Servo Packet Buffer
u8_bit szSerBuffer[11];

int main(void){
      
	delay_ms(0xFF); // Give AVR some time

	cli(); // clear interrupts
	
	//initialise channels to centre 
	channels[0] = 128;
	channels[1] = 128;
	channels[2] = 128;
	channels[3] = 128;
	channels[4] = 128;
	channels[5] = 128;
	channels[6] = 128;
//	channels[7] = 128;
//	channels[8] = 128;
//	channels[9] = 128;

	USART_Init();
	PPM_Init();

	Tx_String(szInitialised);	

	sei();	// set interrupts 

	while(1)
	{
	}

	return 1;
}

/** 
 * Initalises the Pulse Position Modulation (PPM) generator.
 * Sets up the timers and so on.
 */
void PPM_Init()
{
	DDRB |= 0x02; //set pin 1 to output
	PORTB &= 0xFC; // inttialise to 0
	
	unsigned char sreg = SREG; // copy global interrupt flag
	cli();
	TCNT1 = 0x0000;	//initialise timer to zero
	sei();
	SREG = sreg; // restore global interrupt flag
	
	TCCR1A |= 0b01000000; //set OC1A to toggle on compare match 
	TCCR1A &= 0b11111100; //set the mode to 'normal' i.e. not PWM etc.
	TCCR1B &= 0b00000000;
#if PRESCALER == 1
	TCCR1B |= 0b00000001;
#elif PRESCALER == 8
	TCCR1B |= 0b00000010; //set 8 times pre scaler 
#elif PRESCALER == 64
	TCCR1B |= 0b00000011;
#else
	#error "Prescaler chosen was either invalid, or I haven't coded it up."
#endif

	TIMSK |= 0b00010000; // OCIE1A - enable interrupt from timer 1, output compare A

	sreg = SREG;
	cli();
	ppmtime(5000); // initially set timer to 5000us
	sei();
	SREG = sreg;	
}

/**
 * Timer Interrupt Service Routine.  A state machine which generates the PPM signal depending on the channel values. Called immediately after the pin is toggled.
 */
ISR(TIMER1_COMPA_vect)
{
	u8_bit pin = 0b00000010 & PINB;
	
	static u8_bit state = 0;
	u16_bit wait = 0;
	
	if(pin)	// pin is on
	{
		if(state==7) //end of frame
		{
			wait = 5000;
			ppmtime(wait);
		}
		else	//normal pulse
		{
			wait = ppmscale(channels[state]);
			ppmtime(wait);
		}
		state = (state+1)%8; //increment state variable
	}
	else 	// pin is off
	{
		wait = 400;
		ppmtime(wait);
	}
}

/**
 * Initialises the USART communications for receiving commands from the autopilot.
 */
void USART_Init()
{
	// Set the baud rate 
	// is this the baud rate we want defined by SYS_UBRR
	UCSRC &= CLRBIT(URSEL);					/* Allow access to the UBRR register */
	UBRRH = (u8_bit)(SYS_UBRR>>8);			/* Set high bits of 16bit UBRR */
	UBRRL = (u8_bit)(SYS_UBRR);				/* Set low bits of 16bit UBRR */
	
	// Enable the receiver and transmitter and reciever interrupt
	UCSRB |= SETBIT(RXEN);
	UCSRB |= SETBIT(TXEN);
	UCSRB |= SETBIT(RXCIE);
	// Set frame format: 8data, 1stop bit 
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
	UCSRC |= 0b00110000; //set odd parity
}

/**
 * Transmits one byte of data over USART
 */
void Tx_Byte(u8_bit Tx_Data)
{
	// Wait until the transmit buffer is empty 
	while (!( UCSRA & (1<<UDRE)));
	
	// Put data into buffer, sends the data 
	UDR = Tx_Data;

}


void  delay_ms(u8_bit delay)
{
	//scale to 8MHz clock
	u16_bit DelayTime = (delay *  465);
	while(DelayTime > 0)
	{
		DelayTime --;
	}
}

/**
 * Transmit a string null terminated string. Uses the Tx_Byte function.
 */
void Tx_String(u8_bit *PtrPacket)
{
	/* Define Local Variables */
	u8_bit Tx_Data;							/* Char byte to send */
	
	/* Transmit until Null character is detected */
	while((Tx_Data = *PtrPacket) != ('\0'))
	{
		/* Transmit the byte */
		Tx_Byte(Tx_Data);
		
		/* Increment the string point */
		PtrPacket++;
	}

	/* Transmit a new line character and carriage return */
	Tx_Byte('\n');
	Tx_Byte('\r');
}

/** 
 * The interrupt service routine (ISR) for the receipt of a packet. Calls the GetSerPacket() function with the parameter of one received byte.
 */
ISR (USART_RXC_vect)
{
	// Process Byte
/*	if(UCSRA & 0x04)
	{
		Tx_Byte('!');
		Tx_Byte(UDR);
		Tx_Byte('!');
	}
	else
	{
*/
		GetSerPacket(UDR);
//	}
}

/**
 * blahblahblah
 */
void GetSerPacket(u8_bit szByteRx)
{//A state machine that receives telemetry data, maintaining packet synchronisation. Sets the global channel variables with the data it receives.  Please see wiki page, documentation or contact A. Wainwright for specifications of comms.  It is critical the the communications protocol by strictly followed.
	static u8_bit buffer_state = WAIT_FOR_PACKET_START;	//state flag
	static u8_bit sync_state = WAIT_FOR_SYNC0;
	static u8_bit buffer_index = 0;		
	static u8_bit buffer[BUFFER_SIZE];
	int ii;
	static u8_bit bReceive = false;

	
	if(bReceive)
	{
		buffer[buffer_index] = szByteRx;
		buffer_index++;
	}

	//sync machine
	switch(sync_state)
	{
		case WAIT_FOR_SYNC0:
		{
			if(szByteRx == SYNC_BYTE)
			{
				sync_state = WAIT_FOR_SYNC1;
			}
		}
		break;
		
		case WAIT_FOR_SYNC1:
		{	
			if(szByteRx == SYNC_BYTE)
			{
				sync_state = WAIT_FOR_SYNC2;
			}
			else
			{
				sync_state = WAIT_FOR_SYNC0;
			}
		}
		break;
	
		case WAIT_FOR_SYNC2:
		{

			if(szByteRx == SYNC_BYTE)
			{
				sync_state = WAIT_FOR_SYNC3;
			}
			else
			{
				sync_state = WAIT_FOR_SYNC0;
			}
		}
		break;		

		case WAIT_FOR_SYNC3:
		{

			//sync sequence sent
			if(szByteRx == SYNC_BYTE) 
			{
				buffer_index = 0;
				bReceive = true;		
			}
			sync_state = WAIT_FOR_SYNC0;
		}
		break;
	}
	
	//TODO: check that disabling interrupts doesn't screw up the ppm generator
	
	if(buffer_index == BUFFER_SIZE)
	{
		bReceive = false;

		//set the appropriate channels
		cli();
		channels[0] = buffer[0];
		channels[1] = buffer[1]; // port servo
		channels[2] = buffer[2]; // port servo 
		channels[3] = buffer[3]; // fore servo 
		channels[4] = buffer[4]; // rudder  
		channels[5] = buffer[5]; // unknown rudder related channel
		channels[6] = buffer[6]; // starboard servo
		sei();
	}	
}

