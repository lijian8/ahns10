/**
 * @file   arduserial.pde
 * @author Liam O'Sullivan
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Arduino code to transmit to the overo the following:
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
// overo baud rate
#define OVERO_BAUDRATE 115200

// software serial compass object
NewSoftSerial compass(COMPASS_RX,COMPASS_TX);
// compass heading (0.0 - 359.9)
char compass_heading[10];
// compass index
int compass_index = 0;
// compss heading index
int compass_heading_index = 0;

// compass character
char compass_char = 0;
// overo command
char overo_com = 0;
// overo loop counter
int i=0;

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
// Compass sentence: $C256.3P-0.7R-178.5T25.5*29
int readCompass()
{
  if (compass.available() > 0)
  {
    // read the compass character
    compass_char = compass.read();
    // check if a P has been received, terminate compass reading
    if (compass_char == 'P')
    {
     compass_heading_index = compass_index-2;
     compass_index = 0; 
    }
    // if the index is equal or greater than 2 then receiving compass heading
    if (compass_index >= 2)
    {
      compass_heading[compass_index-2] = compass_char;
      compass_index++;
    }
    // check if a $ has been received
    if (compass_char == '$')
    {
      compass_index++;
    }
    // check if a C has been received and that a $ has been received
    if (compass_char == 'C' && compass_index == 1)
    {
      compass_index++;
    }    
  }
}

// read serial data transmitted from the overo and reply with suitable response
int readOvero()
{
  if (Serial.available() > 0)
  {
    overo_com = Serial.read();
    if (overo_com == 'C')
    {
      for(i=0; i<compass_heading_index; i++)
      {
        Serial.print(compass_heading[i],BYTE);
      }     
    }
  } 
}
