/***************************************************************************\
*																			*
*   QUT - AHNS Project														*
*	Scott Osborne															*
*																			*
\***************************************************************************/

#include <avr/io.h>
#include <stdio.h>

#include "Hardware.h"
//#include "Timer.h"
#include <avr/interrupt.h>
//#include "math.h"

//#include "Timer.c"
//#include "UART.h"
//#include "UART.c"

//BAUD rate Calculation
#define FOSC 16000000L// Clock Speed
//#define BAUD 115200

// set to 51 for 19200
// set to 26 for 38400
// set to 8 for 115200
#define MYUBRR 8 //8//FOSC/16/BAUD-1
#define IMUUBRR 26//51//8//FOSC/16/BAUD-1

#define TIMER_RES 12.5

#include <stdint.h>
#include <avr/io.h>

//IMU Definitions

//#define STAOSM 0x5354414F534D0D  //STAOSM in ASCII with 0D
#define NearSensor 1
#define FarSensor 2

#define PRESCALER 1

//Global Variables

int i = 0;
int Byte = 0;
uint8_t Data;


char ChkChar0;
char ChkChar1;
uint8_t TSTAMP1 = 0x00;
uint8_t TSTAMP2 = 0x00;
uint8_t TSTAMP3 = 0x00;
uint8_t TSTAMP4 = 0x00;
uint8_t SYNC1 = 0xFF;
uint8_t SYNC2 = 0xFE;
uint8_t SYNC3 = 0xFD;
uint8_t SYNC4 = 0xFC;

uint8_t ALT = 0x0000;
uint16_t BATL = 0x0000;
uint8_t CRC = 0x43;

uint16_t Xacc = 0;
uint16_t Yacc = 0;
uint16_t Zacc = 0;
uint16_t Xgyr = 0;
uint16_t Ygyr = 0;
uint16_t Zgyr = 0;
uint16_t Xtem = 0;
uint16_t Ytem = 0;
uint16_t Ztem = 0;
uint16_t Alt = 0;

//char wordbuf[32];

char tempChar;
char tempc[33];
int BadChar = 0;
int GoodChar = 0;
int Stat = 0;
int PacketCount = 0;

volatile uint16_t timer = 0x0000;
volatile uint16_t timer_overflow = 0x0000; //100msec
//uint32_t time = 0x00000000;

void UART1_Send16bits(uint16_t Data);

void timer1init()
{
    cli(); //Disable all interupts
    TCNT1 = 0x0000;    //initialise timer to zero
    sei();
   	TCCR1A = 0b00000000; //initialise to zero
	TCCR1B = 0b00000000;
    TCCR1A |= 0b01000000; //set OC1A to toggle on compare match
    TCCR1A &= 0b11111100; //set the mode to 'normal' i.e. not PWM etc.
	
    //TCCR1B &= 0b00001000;
	TCCR1B |= (1<<WGM12); //set CTC mode
	TCCR1B |= (1<<CS0); // Set clk no prescale
	TIMSK = (1<<OCIE1A); //enable output compare match
   	cli();
    OCR1A = ((FOSC/80000)/PRESCALER); // 12.5usec //Check this and adjust offset depending on program time
    timer = 0x0000; //initialise
    timer_overflow = 0x0000;
  	sei(); //reenable interupts
}


ISR(TIMER1_COMPA_vect)
{
cli();
LED_ON( YELLOW );

    if (timer > 0xFFFE)
    {
        timer = 0;
        timer_overflow++;
    }
    else
    {
        timer++;
    }

	sei();
	LED_OFF( YELLOW );
}

void TimeOut_us(int usecs)
{

int time_res = ((FOSC/80000)/PRESCALER)/16;

// Function to wait number of microseconds
int start = (timer_overflow <<16) + timer;
int thetime;
do{
	thetime = (timer_overflow <<16) + timer;	//wait
}while( (thetime - start) < (usecs/time_res)); //350 min for 9600

}

void UART0_vInit(void) //IMU UART
{

// Set baud rate
UBRR0H = (uint8_t)(IMUUBRR>>8);
UBRR0L = (uint8_t)IMUUBRR;
UCSR0C = (2<<UCSZ0);
//UCSR0C |= (1<<USBS0);
// Enable receiver and transmitter
UCSR0B = (1<<RXEN0)|(1<<TXEN0);

}


void UART1_vInit(void) //Radio Link
{

// Set baud rate
UBRR1H = (uint8_t)(MYUBRR>>8);
UBRR1L = (uint8_t)MYUBRR;
// Enable receiver and transmitter
UCSR1B = (1<<RXEN1)|(1<<TXEN1);

}



void UART1_SendByte(uint8_t Data)
{
// Wait if a byte is being transmitted
while((UCSR1A&(1<<UDRE1)) == 0);
//Put data into buffer, sends the data
UDR1 = Data;
}



void UART0_SendByte(uint8_t Data)
{
// Wait if a byte is being transmitted
while((UCSR0A&(1<<UDRE0)) == 0);
//Put data into buffer, sends the data
UDR0 = Data;
}


void UART1_Send16bits(uint16_t Data)
{
uint8_t upperB;
uint8_t lowerB;

lowerB = Data & 0x00FF;
upperB = (Data>>8);
UART1_SendByte(upperB);
UART1_SendByte(lowerB);

}


/*
void UART1_OUTPUT88(uint8_t Upper, uint8_t Lower)
{
uint16_t Temp;

Temp = (Lower | Upper<<8)*4;

Lower = Temp & 0x00FF;
Upper = Temp>>8;
UART1_SendByte(Lower);
UART1_SendByte(Upper);

}
*/


//SPI Init
void SPI_Master_Init() {
/* Set MOSI, SS and SCK output, all others input */
DDRB = (1<<DDB2)|(1<<DDB1)|(1<<DDB0);
/* Enable SPI, Master, set clock rate fck/128 */
SPCR = (1<<SPE)|(1<<MSTR)|(2<<SPR0)|(1<<CPOL)|(1<<CPHA);
//DDRB = (0<<DDB0);
}

void SPI_MasterTransmit(char cData)
{
/* Start transmission */
SPDR = cData;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
}

uint8_t SPI_Transmit(uint8_t data) {
	// set SS low
	PORTB = (0<<PB0);
	//SPDR = (data|0x80);
	SPDR = (data);
	while(!(SPSR & (1<<SPIF)));
	// SS high may be causing the SPI to stop sending
	// set SS high
	PORTB = (1<<PB0);
	return SPDR;
}


// SPI Not efficient but it works
uint16_t SPI_Transmit16(uint8_t address)
{
	uint8_t lower, upper;

	//set CS low
	PORTB = (0<<PB0);
	// send 0,0,6 bit address
	address &= 0b00111111;
	SPDR = address;
	while(!(SPSR & (1<<SPIF)));
	//send 8 bits of junk
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	//Waste time for TSFS

	for (i = 0; i < 25; i++ )
	{
	}
	
	//send 8 junk bits
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	//read high
	upper = SPDR;
	//send 8 junk bits
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	//read low
	lower = SPDR;

	//Set SS High again
	
/* // added to test if LSbyte is after MSByte
	SPDR = 0x00;
	while(!(SPSR & (1<<SPIF)));
	//read high
	upper = SPDR;
	// */
	PORTB = (1<<PB0);
	//return the low+high
	return (lower | upper<<8)*4;
	
}


//Update timestamp function

void UpdateTimeStamp()
{
TSTAMP4 = timer;
TSTAMP3 = timer>>8;
TSTAMP2 = timer_overflow;
TSTAMP1 = timer_overflow>>8;
}


//



char IMU_DataRxChar()
{
while ( !(UCSR0A & (1<<RXC0)) );
return UDR0;
}



uint16_t BuffIMU_DataRx(int index) 
//Gets 4ASCII char converts to a 16bit Binary value and takes comma from the buffer
//char array must be tempc[n]. the value index provides the start of each 4 characters.
{
uint16_t IMU_Data = 0;
GoodChar = 0; //assume char is good
//IMU_Data = 0; //ensure it is reinitialised
for (i=0; i<4; i++)
	{
			tempChar = tempc[index + i];
			//perform ASCII HEX to Binary
			if ((tempChar >= 0x41) && (tempChar <= 0x46)) //Char is A-F
				{
					tempChar = tempChar - 0x37;
					GoodChar = 1;
				}
			else if ((tempChar >= 0x30) && (tempChar <= 0x39)) //Char is 0-9
				{
					tempChar = tempChar - 0x30;
					GoodChar = 1;
				}
			else
				{	
			GoodChar = 0; // bad character
				}
			IMU_Data |= tempChar<<(((4-i)*4)-4);	
	}

return (IMU_Data * GoodChar);

}


void CharBuffTestRun()
{

LED_ON( RED );

UART1_SendByte('S');
UART1_SendByte('T');
UART1_SendByte('A');
UART1_SendByte('O');
UART1_SendByte('S');
UART1_SendByte('M');
UART1_SendByte(0x0D);

UART0_SendByte('S');
UART0_SendByte('T');
UART0_SendByte('A');
UART0_SendByte('O');
UART0_SendByte('S');
UART0_SendByte('M');
UART0_SendByte(0x0D);
LED_OFF( RED );

LED_ON( BLUE );
for (i=0; i<33; i++)
	{
	tempc[i] = IMU_DataRxChar();
	UART1_SendByte(tempc[i]);
	}
LED_OFF( BLUE );

for (i=0; i<5; i++)
	{
	//UART1_SendByte('.');
	}

	//convert temppc[0-3] to Hex
	//update xacc IMU_DataRx()
	Xacc = BuffIMU_DataRx(0);
	UART1_Send16bits(Xacc);
	//test temppc[4] for comma separator

	//convert temppc[5-8] to Hex
	//update yacc
	Yacc = BuffIMU_DataRx(5);
	UART1_Send16bits(Yacc);
	//test temppc[9] for comma separator

	//convert temppc[10-13] to Hex
	//update zacc
	Zacc = BuffIMU_DataRx(10);
	UART1_Send16bits(Zacc);
	//test temppc[14] for comma separator

	//convert temppc[15-18] to Hex
	//update xgry
	Xgyr = BuffIMU_DataRx(15);
	UART1_Send16bits(Xgyr);
	//test temppc[19] for comma separator

	//convert temppc[20-23] to Hex
	//update ygyr
	Ygyr = BuffIMU_DataRx(20);
	UART1_Send16bits(Ygyr);
	//test temppc[24] for comma separator

	//convert temppc[25-28] to Hex
	//update zgyr
	Zgyr = BuffIMU_DataRx(25);
	UART1_Send16bits(Zgyr);
	//test temppc[29] for comma separator
	
	//convert temppc[30-31] to Hex
	//update IMUCRC
	//test temppc[32] for CR
	//test temppc[33] for LF

	int start = (timer_overflow <<16) + timer;
int thetime;
do{
	thetime = (timer_overflow <<16) + timer;	//wait
}while( (thetime - start) < (100000/TIMER_RES)); //350 min for 9600
}



void IMUFreeRun()
{

LED_ON( RED );

UART0_SendByte('S');
UART0_SendByte('T');
UART0_SendByte('A');
UART0_SendByte('F');
UART0_SendByte('R');
UART0_SendByte('M');
UART0_SendByte(0x0D);


while(IMU_DataRxChar() != 0x0D); //get carriage return
while(IMU_DataRxChar() != 0x0A); //get line feed

LED_ON( BLUE );
for (i=0; i<34; i++)
	{
	tempc[i] = IMU_DataRxChar();
	//UART1_SendByte(tempc[i]);
	}
LED_OFF( BLUE );

	//convert temppc[0-3] to Hex
	//update xacc IMU_DataRx()
	Xgyr = BuffIMU_DataRx(0);
	//test temppc[4] for comma separator

	//convert temppc[5-8] to Hex
	//update yacc
	Ygyr = BuffIMU_DataRx(5);
	//test temppc[9] for comma separator

	//convert temppc[10-13] to Hex
	//update zacc
	Zgyr = BuffIMU_DataRx(10);
	//test temppc[14] for comma separator

	//convert temppc[15-18] to Hex
	//update xgry
	Xacc = BuffIMU_DataRx(15);
	//test temppc[19] for comma separator

	//convert temppc[20-23] to Hex
	//update ygyr
	Yacc = BuffIMU_DataRx(20);
	//test temppc[24] for comma separator

	//convert temppc[25-28] to Hex
	//update zgyr
	Zacc = BuffIMU_DataRx(25);
	//test temppc[29] for comma separator
	
	//convert temppc[30-31] to Hex
	//update IMUCRC
	//test temppc[32] for CR
	//test temppc[33] for LF

LED_OFF( RED );

}



uint16_t IMU_DataRx() 
//Gets 4ASCII char converts to a 16bit Binary value and takes comma from UART0
{
uint16_t IMU_Data =0;
GoodChar = 0;
//IMU_Data = 0; //ensure it is reinitialised
for (i=0; i<4; i++)
	{
	while ( !(UCSR0A & (1<<RXC0)) );
		//while ( UDR0 != 0x0D ); //Test for end of string
			tempChar = UDR0;
			//perform ASCII HEX to Binary
			if ((tempChar >= 0x41) && (tempChar <= 0x46)) //Char is A-F
				{
					tempChar = tempChar - 0x37;
					GoodChar = 1;
				}
			else if ((tempChar >= 0x30) && (tempChar <= 0x39)) //Char is 0-9
				{
					tempChar = tempChar - 0x30;
					GoodChar = 1;
				}
			else
				{	
			GoodChar = 0; // bad character
				}
			IMU_Data |= tempChar<<(((4-i)*4)-4);	
	}
	
while ( !(UCSR0A & (1<<RXC0)) );
tempChar = UDR0; //This takes the comma separator from the buffer

return IMU_Data;

}


int IMUGetData()
{
GoodChar = 0;
// Send Start one shot mode command to SD 6DOF IMU

UART0_SendByte('S');
UART0_SendByte('T');
UART0_SendByte('A');
UART0_SendByte('O');
UART0_SendByte('S');
UART0_SendByte('M');
UART0_SendByte(0x0D);


/*for (i=0; i<4; i++)
{
	while ( !(UCSR0A & (1<<RXC0)) );
		while ( UDR0 != 0x0D ); //Test for end of string
	 	Xacc |= UDR0<<((((4-i)*8)-8)); //Ensure characters are in the correct order
}*/


Xacc = IMU_DataRx();
Xacc = GoodChar * Xacc;
//if (BadChar != 0) return BadChar;
Yacc = IMU_DataRx();
Yacc = GoodChar * Yacc;
//if (BadChar != 0) return BadChar;
Zacc = IMU_DataRx();
Zacc = GoodChar * Zacc;
//if (BadChar != 0) return BadChar;
Xgyr = IMU_DataRx();
Xgyr = GoodChar * Xgyr;
//if (BadChar != 0) return BadChar;
Ygyr = IMU_DataRx();
Ygyr = GoodChar * Ygyr;
//if (BadChar != 0) return BadChar;
Zgyr = IMU_DataRx();
Zgyr = GoodChar * Zgyr;
//if (BadChar != 0) return BadChar;

//2char Checksum and 0x0D termiator
while ( !(UCSR0A & (1<<RXC0)) );
tempChar = UDR0; //This takes the comma separator from the buffer
while ( !(UCSR0A & (1<<RXC0)) );
ChkChar0 = UDR0; //This takes the first Checksum Char from the buffer
while ( !(UCSR0A & (1<<RXC0)) );
ChkChar1 = UDR0; //This takes the next Checksum char from the buffer
while ( !(UCSR0A & (1<<RXC0)) );
tempChar = UDR0; //This takes the 0x0D char from the buffer
while ( !(UCSR0A & (1<<RXC0)) );
tempChar = UDR0; //This takes the 0x0A char from the buffer

return 0;
/*for (i=0; i<4; i++)
{
	while ( !(UCSR0A & (1<<RXC0)) );
		while ( UDR0 != 0x0D ); //Test for end of string
	 	Zacc |= UDR0<<((((4-i)*8)-8));
}*/

}


/*void IMUGetDataWord()
{
	//UART0_TxByte(STAOSM);
	for (i = 0; i <32; i++)
	{
	while ( !(UCSR0A & (1<<RXC0)) );
	while ( UDR0 != 0x0D );
	wordbuf[i] = UDR0;
	}

	for (j = 0; j < 32; j++) // j is count of values
	{
		for ( i = 0; i < 4; i++ ) //read in 4 chars at a time
        	{
			while (wordbuf[j] != ',')
			data |= wordbuf[j]<<((4-i)*8);
			}
	}

	for (i = 0; i < 32; i++)
	{
	if (wordbuf[i] < 0x41)
		hexbuf[i] = wordbuf[i] - 0x30;
	else
		hexbuf[i] = wordbuf[i] - 0x31;
	}

}*/


/*uint16_t GetADCVal(int ADC_Port)
{
	uint16_t AltData =0;
	int Err_fl =0;
	ADCSRA |= (1<<ADEN)|(1<<ADSC);//enable and leave prescalar as default 2?? 
	if (ADC_Port == 1)
	ADMUX |= (1<<MUX0);
	else if (ADC_Port == 2)
	ADMUX |= (1<<MUX1);
	else Err_fl = 1;
	
	while (ADSC == 1); //Check if ready to read
	ADCSRA = (1<<ADSC);

	while (ADIF != 1); //ADIF set when conversion complete
	{
	AltData |= ADCL;
	AltData |= ADCH<<8;
	}
	
	ADCSRA = (0<<ADSC);
	
	return AltData;

}
*/

void InitADC()
{
ADMUX=(1<<REFS0)|(1<<REFS1);                         // For Aref=Internal 2.56V;
ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0); //Prescalar div factor =
}


uint16_t ADCVal(uint8_t ch)
{

  ADCSRA|=(0<<ADSC); //Dont start conversion yet
//Select ADC Channel ch must be 0-7
	ADMUX=ADMUX&0b11100000; //clear ADMUX channel selection
  	ch=ch&0b00000111;
  	ADMUX|=ch;


   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   ADCSRA|=(1<<ADIF);

   return(ADC);


}

/*uint16_t Volts2Alt(uint16_t Voltage)
{
uint16_t TempAlt;

switch (Voltage)
{
case 0x3FF:
TempAlt = 500
}


}*/



int main(void)
{


//initial setup
timer1init();
//timer0init();
UART1_vInit();
UART0_vInit();
//SPI_Master_Init();
//InitTimer;



ASSR &= ~( 1 << AS0 );  // Make sure Port G LED pins are setup for I/O

RED_LED_DDR     |= RED_LED_MASK;
YELLOW_LED_DDR  |= YELLOW_LED_MASK;
BLUE_LED_DDR    |= BLUE_LED_MASK;

InitADC();

//Configure IMU to start free run mode


UART1_SendByte('S');
UART1_SendByte('T');
UART1_SendByte('A');
UART1_SendByte('F');
UART1_SendByte('R');
UART1_SendByte('M');
UART1_SendByte(0x0D);

UART0_SendByte('S');
UART0_SendByte('T');
UART0_SendByte('A');
UART0_SendByte('F');
UART0_SendByte('R');
UART0_SendByte('M');
UART0_SendByte(0x0D);



while ( 1 )
{

/*PacketCount = 0; //Initialise Packet Count

while (PacketCount < 12)
{
*/
	//TStamp++;

//Clear LED Status
//	LED_OFF( RED );
//    LED_OFF( BLUE );
	//LED_OFF( YELLOW );


// 2/7/09 swapover


LED_ON( BLUE );
LED_OFF( RED );

//Alt = GetADCVal(NearSensor); // Get Short Distance Alt
//FarAlt = GetADCVal(FarSensor); // Get Long Distance Alt
//UpdateTimeStamp(); //Update the time stamp


IMUFreeRun();

/*LED_ON( BLUE );
for (i=0; i<33; i++)
	{
	UART1_SendByte(IMU_DataRxChar());
	}
LED_OFF( BLUE );*/


//Alt = GetADCVal(NearSensor);

Alt = ADCVal(0x00); //This is good
//BATL = ADCVal(0x04); //this is good

//while (IMUGetData()); //Update IMU Data


//UART1_SendByte(0x0D);

/*
UART1_SendByte(0x30);
Stat = IMUGetData();
UART1_SendByte(0x53);
UART1_SendByte(0x3A);
UART1_SendByte(Stat);
UART1_SendByte(0x3A);
*/

//UpdateTimeStamp(); //Update the time stamp


//22 IMU testrun
LED_OFF( BLUE );
LED_ON( RED );

	UART1_SendByte(SYNC1);
	UART1_SendByte(SYNC2);
	UART1_SendByte(SYNC3);
	UART1_SendByte(SYNC4);
	//UART1_SendByte(TSTAMP1);
//	UART1_SendByte(TSTAMP2);
//	UART1_SendByte(TSTAMP3);
//	UART1_SendByte(TSTAMP4);

	UART1_Send16bits(timer_overflow & 0x7FF); //Remove MSB for ICD conformance);
	UART1_Send16bits(timer);
	//  - Removed for Altitude hold testing
	UART1_Send16bits(Xacc); //& 0x7FF);
	UART1_Send16bits(Yacc); //& 0x7FF);
	UART1_Send16bits(Zacc); //& 0x7FF);
	UART1_Send16bits(Xgyr); //& 0x7FF);
	UART1_Send16bits(Ygyr); //& 0x7FF);
	UART1_Send16bits(Zgyr); //& 0x7FF);
	//UART1_Send16bits(Xtem); 
	//UART1_Send16bits(Ytem);
	//UART1_Send16bits(Ztem);*/
	UART1_Send16bits(Alt & 0x7FF);
	//UART1_Send16bits(BATL & 0x7FF);
	UART1_SendByte(CRC);
	UART1_SendByte(CRC);

	LED_ON( BLUE ); //22 IMU testrun

	/*PacketCount++;
}*/


// Wait to send data
int start = (timer_overflow <<16) + timer;
int thetime;
do{
	thetime = (timer_overflow <<16) + timer;	//wait
}while( (thetime - start) < (2000/TIMER_RES)); //1000

LED_OFF( RED );

	}
return 0;
}
