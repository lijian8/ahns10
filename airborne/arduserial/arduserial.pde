/*
 * arduserial.pde
 * 
 * Description: Arduino code to transmit to the overo the following:
 * 1. connect to compass and extract heading data
 * 2. connect to camera and gather image processing data
 * 3. connect to ultrasonic sensor and request height data
 * 4. perform adc read on battery voltage
 *
 */

#include "NewSoftSerial.h"

// compass transmit pin
#define COMPASS_TX 10
// compass receive pin
#define COMPASS_RX 11
// compass baud rate
#define COMPASS_BAUDRATE 38400
// compass message length
#define COMPASS_MSG_LEN 29
// compass heading length
#define COMPASS_HEADING_LEN 5
// compass heading offset
#define COMPASS_HEADING_OFFSET 2
// overo baud rate
#define OVERO_BAUDRATE 115200

// software serial compass object
NewSoftSerial compass(COMPASS_RX,COMPASS_TX);
// compass buffer
char compass_buffer[29];
// buffer index
int compass_index = 0;
// compass heading
char compass_heading[5];
// compass for loop variable
int i = 0;
// compass while loop variable
int j = 0;

// overo command
char overo_com = 0;

// arduserial functions
// read the serial compass data
int readCompass();
// read the overo data
int readOvero();

void setup()   
{
  Serial.begin(OVERO_BAUDRATE);
  pinMode(COMPASS_TX, OUTPUT);
  pinMode(COMPASS_RX, INPUT);
  // software serial (receive pin, transmit pin)
  compass.begin(COMPASS_BAUDRATE);
}


void loop()                     
{
  readCompass();
  readOvero();
}

// read serial data transmitted from the compass
int readCompass()
{
  if (compass.available() > 0)
  {
    // add to character to compass buffer
    compass_buffer[compass_index] = compass.read();
    //Serial.print(compass_buffer[compass_index]);
    compass_index++;
    if (compass_index == COMPASS_MSG_LEN)
    {
      // compass packet has been received, update the heading
      // search for the heading in the message (need to do this since the message can glitch
      while (compass_buffer[j] != '$')
      {
        j++;
      }
      for(i=0; i<COMPASS_HEADING_LEN; i++)
      {
        compass_heading[i] = compass_buffer[COMPASS_HEADING_OFFSET+j+i];
      }
      // reset the indexes
      compass_index = 0; 
      j = 0;
    }
  }
}

//$C256.3P-0.7R-178.5T25.5*29


// read serial data transmitted from the overo and reply with suitable response
int readOvero()
{
  if (Serial.available() > 0)
  {
    overo_com = Serial.read();
    if (overo_com == 'C')
    {
      // print the compass heading
      for(i=0; i<COMPASS_HEADING_LEN; i++)
      {
        Serial.print(compass_heading[i]);
      }
    }
  } 
  
}
