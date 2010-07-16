/**
 * \file   mcu.c
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-06-10 23:59:05 +1000 (Thu, 10 Jun 2010) $
 * $Rev: 164 $
 * $Id: main.cpp 164 2010-06-10 13:59:05Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Main file for the ATMEGA328P Mode Control Unit
 */

#define F_CPU 8000000UL // 16MHz scaled to 8 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Mode LED Indicator Defines
#define GREEN_LED 3
#define BLUE_LED 4
#define RED_LED 2

// ESC Commands
#define ESC1 OCR0A
#define ESC2 OCR0B
#define ESC3 OCR1A
#define ESC4 OCR1B

uint8_t escCommands[4] = {0, 0, 0, 0};

// PWM Defines
#define F_PWM ((double) F_CPU/(64.0*510.0))
#define PWM_MAX_US 2000
#define PWM_MIN_US 600

// Boolian Defines
#define TRUE 1
#define FALSE 0
#define SBR(x) (1<<(x))   /* Set bit 'x' in register */
#define CBR(x) ~(1<<(x))  /* Clear bit 'x' in register */
#define BRS(r,x) (((1 << (x)) & ((r) & (1 << (x)))) == 1) /* determine if bit 'x' on register 'r' is set */

void init();
uint8_t InitialiseModeIndicator();

// Serial Comms
uint8_t InitialiseUSART();

// Timer Functions
uint8_t InitialiseTimer0();
uint8_t InitialiseTimer1();

// PWM Functions
uint8_t StartPWM();
uint8_t StopPWM();

// Input Pins for PC
#define NUM_CHANNELS 6
#define CHANNEL1 5
#define CHANNEL2 4
#define CHANNEL3 3
#define CHANNEL4 2
#define CHANNEL5 1
#define CHANNEL6 0
uint8_t InitialisePC();

// Inputs for PC
typedef struct
{
  uint8_t isHigh;
  uint32_t startTimerCount;
  uint32_t overflowCount;
  uint32_t measuredPulseWidth;
} Channel;

volatile Channel inputChannels[NUM_CHANNELS];
volatile uint8_t risenPINC;
volatile uint8_t fallenPINC;
volatile uint8_t countChanged;

inline void ProcessPC();

// Timer2 for PC Pulse Width Timing
uint8_t InitialiseTimer2();


void main (void)
{
  StopPWM();
  init();
  StartPWM();

  for ( ; ; )
  {
    ProcessPC();
  }

  return;
}

void ProcessPC()
{
  uint8_t i = 0;
  if ((risenPINC != 0) || (fallenPINC != 0))  // process input capture
  {
    for (i = 0; i < NUM_CHANNELS; ++i)
    {
      if (BRS(risenPINC,i)) // channel i changed to high
      {
        risenPINC &= CBR(i);

        if (!inputChannels[i].isHigh) // not known high - ok
        {
          inputChannels[i].isHigh = 1;
          inputChannels[i].startTimerCount = countChanged;
        }
        else // known high - lost falling edge
        {
          inputChannels[i].isHigh = 1;
          inputChannels[i].startTimerCount = countChanged;
          inputChannels[i].overflowCount = 0;
        }
      }
      else if (BRS(fallenPINC,i)) // channel i changed to low
      {
        fallenPINC &= CBR(i);
        if (inputChannels[i].isHigh) // not known low so tracked - ok
        {
          inputChannels[i].isHigh = 0;
          inputChannels[i].measuredPulseWidth = (256 - inputChannels[i].startTimerCount) + (inputChannels[i].overflowCount - 1)*256 + countChanged;
          inputChannels[i].startTimerCount = 0;
	  inputChannels[i].overflowCount = 0;
        }
        //else known low - missed rising edge ignore
       }
    }
  }

  return;
}

void init()
{
  // Setup Clock - prescalar of 2 to 8MHz
  CLKPR = (1<<CLKPCE);
  CLKPR = (1<<CLKPS0);
  
  InitialiseModeIndicator();
  InitialiseTimer0();
  InitialiseTimer1();


  InitialiseUSART();

  InitialiseTimer2();
  InitialisePC();
  
  sei();

  return;
}


uint8_t InitialiseModeIndicator()
{
  DDRD |= (1<<RED_LED) | (1<<GREEN_LED) | (1<<BLUE_LED);
  
  uint8_t i = 0;
  for (i = 0; i < 4; ++i)
  {
    PORTD ^= (1<<RED_LED) | (1<<GREEN_LED) | (1<<BLUE_LED);
    _delay_ms(250);
  }

  return 1;
}

uint8_t InitialiseTimer0()
{
  // Set Initialisation of Timer
  OCR0A = 0;
  OCR0B = 0;
  
  // Register A 
  // COM0A = 10 for Non-inverting PWM
  // COM0B = 10
  // WGM0_210 = 001 for Normal PWM
  TCCR0A = (1 << COM0A1) | (0 << COM0A0) | 
           (1 << COM0B1) | (0 << COM0B0) | 
           (0 << WGM01) | (1 << WGM00);

  // Register B
  // No Clock Source Yet
  TCCR0B = (0 << WGM02); 

  // Outputs - OCR0A and OCR0B
  DDRD |= (1 << 6) | (1 << 5);

  return 1;
}

uint8_t InitialiseTimer1()
{  
  // Set Initialisation of Timer
  OCR1A = 0;
  OCR1B = 0;
  
  // Register A 
  // COM1A = 10 for Non-inverting PWM
  // COM1B = 10
  // WGM0_3210 = 0001 for 8bit Normal PWM
  TCCR1A = (1 << COM1A1) | (0 << COM1A0) | 
           (1 << COM1B1) | (0 << COM1B0) | 
           (0 << WGM11) | (1 << WGM10);

  // Register B
  // No Clock Source Yet
  TCCR1B = (0 << WGM13) | (0 << WGM12); 

  // Outputs - OCR1A and OCR1B
  DDRB |= (1 << 1) | (1 << 2);

  return 1;
}

uint8_t InitialiseTimer2()
{
  // Clear Asynchronous Source
  ASSR = 0;

  // Register A
  // COM2A_10 = 00 for normal mode
  // WGM2_210 = 000 for normal mode
  TCCR2A = (0 << COM2A1) | (0 << COM2A0) | 
           (0 << WGM21) | (0 << WGM20);

  // Register B
  // CS2_210 = 010 for Prescale or 8 to 1MHz
  TCCR2B = (0 << WGM22) | (0 << CS22) | (1 << CS21) | (0 << CS20);

  // Interrupt Mask
  // TOIE2 - for Timer overflow
  //TIMSK2 = 0;
  TIMSK2 |= (1 << TOIE2);

  return 1;
}	

uint8_t StopPWM()
{
  // ESC 1 and 2
  OCR0A = 0;
  OCR0B = 0;
  
  // ESC 3 and 4
  OCR1A = 0;
  OCR1B = 0;

  // Disable Clocks
  TCCR0B &= (0 << CS02) | (0 << CS01) | (0 << CS00);
  TCCR1B &= (0 << CS12) | (0 << CS11) | (0 << CS10);

  return 1;
}

uint8_t StartPWM()
{
  // CS0_210 = 011 for N = 64
  TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);

  OCR0A = 0;
  OCR0B = 0;
  OCR1A = 0;
  OCR1B = 0;

  return 1;
}

uint8_t InitialiseUSART()
{
  uint8_t bRet = 1;

  return bRet;
}

uint8_t InitialisePC()
{
  // Pins to Inputs to enable read of high or low
  DDRC = (1 << CHANNEL1) | (1 << CHANNEL2) | (1 << CHANNEL3) | (1 << CHANNEL4) | (1 << CHANNEL5) | (1 << CHANNEL6);
 
  // PC Interrupt Control Reg
  PCICR = (1 << PCIE1);

  // PC Interrupt MASK
  // Interrupts for the Pins all in PCMSK1
  PCMSK1 = (1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8);
   
  return 1;
}

ISR(PCINT1_vect)
{
  static uint8_t previousPINC = 0;
  uint8_t changedPINC = 0;
  uint8_t i = 0;

  // Get Changed Time
  countChanged = TCNT2;

  // Flag the Changes
  changedPINC = PINC ^ previousPINC;
  
  // Determine Falling or Rising Edge
  // TODO Only Check Changed Channels
  for (i = 0; i < NUM_CHANNELS; ++i)
  {
    if (BRS(changedPINC,i))
    {
      if (BRS(previousPINC,i)) // previously high
      {
        fallenPINC |= SBR(i);
	risenPINC &= CBR(i);
      }
      else // previously low
      {
        risenPINC |= SBR(i);
	fallenPINC &= CBR(i);
      }
    }
  }

  // Update Previous
  previousPINC = PINC;
}


ISR(TIMER2_OVF_vect)
{
  uint8_t i = 0;
  
  // Increment Timer Overflow Counts of High Pulses
  for (i = 0; i < NUM_CHANNELS; ++i)
  {
    if (inputChannels[i].isHigh)
    {
      inputChannels[i].overflowCount++;
    }
  }
}
