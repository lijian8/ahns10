/* Libraries Included */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

/* Type definitions */
typedef unsigned char u8_bit;				/* Define 8 bit data type */
typedef unsigned int u16_bit;				/* Define 16 bit data type */
typedef unsigned long u32_bit;			/* Define 32 bit data type */


/* Global Definitions */
#define FOSC 8000000							/* System clock frequency */
#define BAUD 19200							/* Serial Baud Rate */
#define SYS_UBRR FOSC/16/BAUD-1			/* USART Baud Rate Register Value */
#define WAIT_FOR_PACKET_START 1			/* State indication flag */
#define WAIT_FOR_PACKET_END -1			/* State indication flag */
											
#define MaxOCR1A 1850/2						/* Maximum OCR1A value calculated as below */
#define MinOCR1A 850/2							/* Minimum OCR1A value calculated as below */
#define MaxOCR1B 1200
#define MinOCR1B 500
#define OCR1A_Scaler (MaxOCR1A-MinOCR1A)/180  		/* This value is used to scale the PWM signals to the servos */
#define OCR1B_Scaler (MaxOCR1B-MinOCR1B)/180			/*it must be calculated by finding the max + min OCR1A/B
																	values for the servos, then finding the difference and dividing by 
																	180 degrees. This must be calculated for use of different servos */
	


/* Global Strings */
u8_bit szInitialised[] = "..AVR INITIALISED..";	/* Initialised Lable */
u8_bit szResendData[] = "$ResendData";			/* Request for retranmission of servo data */
u8_bit szErrorMsg[] = "$ERROR";					/* Error msg generated due to unknown packet error */

/* Servo Packet Buffer */
u8_bit szSerBuffer[11];

u16_bit Pan;
u16_bit Tilt;


/* Macro Definitions */
#define SETBIT(x) (1<<(x))					/* Set bit 'x' in register */
#define CLRBIT(x) ~(1<<(x))					/* Clear bit 'x' in register */				


/* Function Prototypes */
void USART_Init();
void PWM_Init();
void delay_ms(u8_bit delay);
void Tx_Byte(u8_bit Tx_Data);
void Tx_String(u8_bit *Packet);
void Send_Servo_Info();
void Set_PWM_Length(u8_bit azi_serv, u8_bit ele_serv);
void GetSerPacket(u8_bit szByteRx);
void SetSerPos(u16_bit Ser1, u16_bit Ser2);

/* Main Function */
int main(void)
{
	/* Delay to allow AVR to settle */
	delay_ms(0xFF);
	
	/* Clear interrupts */
	cli();
	
	/* Initialise the USART and PWM */
	USART_Init();
	PWM_Init();

	/* Display the initialisation lable */
	Tx_String(szInitialised);


	/* Set interrupts */
	sei();

	/* Wait indefinately */
	while(1);

	/* Exit the program */
	return 0;
}

/* Function Definitions */
void USART_Init()
{
	/* Set the baud rate */
	// is this the baud rate we want defined by SYS_UBRR
	UCSRC &= CLRBIT(URSEL);					/* Allow access to the UBRR register */
	UBRRH = (u8_bit)(SYS_UBRR>>8);			/* Set high bits of 16bit UBRR */
	UBRRL = (u8_bit)(SYS_UBRR);				/* Set low bits of 16bit UBRR */
	
	/* Enable the receiver and transmitter and reciever interrupt*/
	UCSRB |= SETBIT(RXEN);
	UCSRB |= SETBIT(TXEN);
	UCSRB |= SETBIT(RXCIE);
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
	
	
}

void Tx_Byte(u8_bit Tx_Data)
{
	/* Wait until the transmit buffer is empty */
	while (!( UCSRA & (1<<UDRE)));
	
	/* Put data into buffer, sends the data */
	UDR = Tx_Data;

}

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

	
void PWM_Init()
{
	/* Set the PortB pins to output (OC1A & OC1B) */
	DDRB |= SETBIT(PB1)|SETBIT(PB2);

	/* Reset the clock */
	TCNT1H = 0x00;
	TCNT1L = 0x00;

	/* Set the output compare mode to clear on compare match counting up */
	// not really sure
	TCCR1A |= SETBIT(COM1A1);
	TCCR1A &= CLRBIT(COM1A0);
	TCCR1A |= SETBIT(COM1B1);
	TCCR1A &= CLRBIT(COM1B0);	
	
	/* Clear force output compare match */
	// not really sure
	TCCR1A &= CLRBIT(FOC1A);
	TCCR1A &= CLRBIT(FOC1B);

	/* Set the wave generation mode to PWM, "Phase and Frequency Correct" & "ICR1 = TOP" */
	// for fast PWM with OCR1A defining TOP of counter, set 1 1 1 1, ...1110 for ICR1
	TCCR1B |= SETBIT(WGM13);
	TCCR1B &= CLRBIT(WGM12);
	TCCR1A &= CLRBIT(WGM11);
	TCCR1A &= CLRBIT(WGM10);

	/* Clear Input Capture Noise Canceler */
	TCCR1B &= CLRBIT(ICNC1);

	/* Set the clock prescaler to divide by 8 */
	// CRO measured 1.55ms duty cycle or 64.516129hz. This chip runs at max 16Mhz. thus duty cycle divided into 24800 sections with no prescaler 
	TCCR1B &= CLRBIT(CS12);
	TCCR1B |= SETBIT(CS11);
	TCCR1B &= CLRBIT(CS10);

	/* Set the TOP value to 10000 */
	ICR1 = 10000;

	OCR1A = (MaxOCR1A - MinOCR1A)/2 + MinOCR1A;
	OCR1B = (MaxOCR1B - MinOCR1B)/2 + MinOCR1B;

	//Send_Servo_Info();


}

void Send_Servo_Info()
{
	/* Local Buffers */
	u8_bit SRV1[15];
	u8_bit SRV2[15];

	/* Convert PWM values to strings */
	sprintf(SRV1, "Servo 1: %d", OCR1A);
	sprintf(SRV2, "Servo 2: %d", OCR1B);
	
	/* Transmit the info */
	Tx_String(SRV1);
	Tx_String(SRV2);

}

void delay_ms(u8_bit delay)
{
	/* Scale the delay time dependant on 8MHz clk */
	u16_bit DelayTime = (delay * 465);
	while(DelayTime != 0){
		DelayTime--;
	}

}
	
	 
ISR (USART_RXC_vect)
{
	/* Process Byte */
	GetSerPacket(UDR);
}

void GetSerPacket(u8_bit szByteRx)
{

	  static u16_bit iSrv_Packet_State = WAIT_FOR_PACKET_START;	/* State indication flag */
	  static u16_bit iPacketIndex = 0;							/* Packet Buffer Index */

	  /* Use state machine to buffer the packet */
	  switch (iSrv_Packet_State){
	  	case WAIT_FOR_PACKET_START:
		{
			/* Check to see if we are at the begining of a packet */
			if (szByteRx == '*'){
				/* Store the value */
				szSerBuffer[iPacketIndex] = szByteRx;

				/* Increment the index */
				iPacketIndex++;

				/* Change the state */
				iSrv_Packet_State = WAIT_FOR_PACKET_END;
			}
		}
		break;

		case WAIT_FOR_PACKET_END:
		{
			/* Buffer until the end of a packet */
			if((szByteRx != '#') && (iPacketIndex < 9)){
				/* Store the value */
				szSerBuffer[iPacketIndex] = szByteRx;

				/* Increment the index */
				iPacketIndex++;
			}
			else if((szByteRx == '#') && (iPacketIndex <= 9)){
				/* Store the last character */
				szSerBuffer[9] = szByteRx;

				/* Store the Null character */
				szSerBuffer[10] = '\0';

				/* Reset the state */
				iSrv_Packet_State = WAIT_FOR_PACKET_START;

				/* Reset the index */
				iPacketIndex = 0;

				Pan = 100 * (szSerBuffer[1]-48) + 10 * (szSerBuffer[2]-48) + 1*(szSerBuffer[3]-48) + 0.1*(szSerBuffer[4]-48);
				
				Tilt= 100 * (szSerBuffer[5]-48) + 10 * (szSerBuffer[6]-48) + 1*(szSerBuffer[7]-48) + 0.1*(szSerBuffer[8]-48);
								
				Tx_String(szSerBuffer);				
				
				/* Set the servo positions */
				//SetSerPos(szSerBuffer[1], szSerBuffer[2]);
				SetSerPos(Pan, Tilt);
			}
			else{
				/* Buffer overrun reset the settings */
				iSrv_Packet_State = WAIT_FOR_PACKET_START;

				/* Reset the index */
				iPacketIndex = 0;

				/* Request a retransmission of servo data */
				Tx_String(szResendData);
			}
		}
		break;
		default:
		{
			/* error occured */
			Tx_String(szErrorMsg);

		}
	}
}

void SetSerPos(u16_bit Ser1, u16_bit Ser2)
{	
	u16_bit Ser1PWM = Ser1;
	u16_bit Ser2PWM = Ser2;	

	/* Convert the argument of degrees into pulse width for pan*/
	Ser1PWM = Ser1PWM*5.59444 + 893;

	/* 
	5.59444 is the (range of pulse width required to move 180 degrees)/180, that is:

							5.59444 = (1900-893)/180; 

	where the values 1900-893 correspond yo 180 degrees movement in the range of pulse width 
	the servo requires to move from end point to end point.
	*/

	/* Convert the argument of degrees into pulse width for tilt*/
	
	Ser2PWM = Ser2PWM*10.77777 + 660;

	/*
	As above

							10.77777 = (1630-660)/90;

	*/			

	/* Set the PWM signal to the servos */
	//u16_bit Ser1PWM = (MaxOCR1A - Ser1*OCR1A_Scaler);
	//u16_bit Ser2PWM = (MinOCR1B + Ser2*OCR1B_Scaler);	

	/* Check that that values sent are within the limits */
	if((Ser1PWM <= 1905) && (Ser1PWM >= 890)){
		/* Set the servo position */
		OCR1A = Ser1PWM/2;
	}
	else{
		Tx_String("Outside allowed range:: PAN.");
	}
	if((Ser2PWM <= 1635) && (Ser2PWM >= 655)){
		/* Set the servo position */
		OCR1B = Ser2PWM/2;
	}
	else{
		Tx_String("Outside allowed range:: TILT.");
	}

}
