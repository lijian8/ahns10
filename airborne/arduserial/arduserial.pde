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
#define COMPASS_TX 4
// compass receive pin
#define COMPASS_RX 5
// battery voltage adc pin
#define VOLTAGE_ADC 7
// altitude sensor adc pin
#define ALTITUDE_ADC 6
// compass baud rate
#define COMPASS_BAUDRATE 19200
// overo baud rate
#define OVERO_BAUDRATE 115200
// battery voltage read time
#define ADC_READ 5

// software serial compass object
NewSoftSerial compass(COMPASS_RX,COMPASS_TX);
// compass heading (0.0 - 359.9)
char compass_heading[10];
// compass buffer
char compass_buffer[10];
// compass index
int compass_index = 0;
// compss heading index
int compass_heading_index = 0;
// compass buffer index
int compass_buffer_index = 0;
// compass index difference
int compass_index_diff = 0;

// compass character
char compass_char = 0;
// overo command
char overo_com = 0;
// overo loop counter
int i=0;
// battery voltage reading
int adc_read = 0;
// battery voltage raw
double bat_voltage_raw = 0;
// transmitted battery voltage
double bat_voltage = 0;
// transmitted altitude reading
double altitude = 0;
// time counter adc
unsigned long current_time;
// previous time 
unsigned long previous_time;
// battery voltage array
double bat_voltage_avg[5];
// battery voltage avg counter
int bat_voltage_avg_count = 0;
// altitude sensor array
double altitude_avg[5];
// altitude voltage
int altitude_avg_count = 0;
// moving average counter
double average = 0.0;

// arduserial functions
// read the serial compass data
int readCompass();
// read the battery voltage
int readVoltage();
// read the overo data
int printOvero();

void setup()   
{
  // setup delay time
  //delay(35000);
  Serial.begin(OVERO_BAUDRATE);
  pinMode(COMPASS_TX, OUTPUT);
  pinMode(COMPASS_RX, INPUT);
  // software serial (receive pin, transmit pin)
  compass.begin(COMPASS_BAUDRATE);
  // init the time
  previous_time = millis();
}


void loop()                     
{
  // check the time
  current_time = millis() - previous_time;
  readCompass();
  if (current_time > ADC_READ)
  {
      readVoltage();
      readAltitude();
      printOvero();
      previous_time = millis();
  }
}

// read serial data transmitted from the compass
// Compass sentence: $C256.3P-0.7R-178.5T25.5*29
int readCompass()
{
  if (compass.available() > 0)
  {
    // read the compass character
    compass_char = compass.read();
    if (compass_char != '\n')
    {
      compass_buffer[compass_buffer_index++] = compass_char;
    }
    if (compass_char == '\n')
    {
      compass_heading_index = compass_buffer_index; 
      for(i=0; i<compass_heading_index; i++)
      {
        compass_heading[i] = compass_buffer[i];
      }
      compass_buffer_index = 0;
    }
  }
}

// read the battery voltage via ADC
int readVoltage()
{
   adc_read = analogRead(VOLTAGE_ADC);
   bat_voltage_raw = (adc_read/double(1023))*5;
   bat_voltage = 2.7964*bat_voltage_raw + 0.103465;
   // store the voltage data in an array
   bat_voltage_avg[bat_voltage_avg_count] = bat_voltage;
   bat_voltage_avg_count++;
   if (bat_voltage_avg_count > 4)
   {
    bat_voltage_avg_count = 0; 
   }
   average = 0;
   // compute the moving avg
   for(i=0; i<5; i++)
   {
     average = average + bat_voltage_avg[i];
   }
   // store the filter battery voltage
   bat_voltage = average/5.0;
}

// read the altitude sensor volrage via ADC
int readAltitude()
{
  adc_read = analogRead(ALTITUDE_ADC); 
  altitude = 0.0135*adc_read + 0.04155;
  // store the voltage data in an array
  /*altitude_avg[altitude_avg_count] = altitude;
  altitude_avg_count++;
  if (altitude_avg_count > 4)
  {
   altitude_avg_count = 0; 
  }
  average = 0;
  // compute the moving avg
  for(i=0; i<5; i++)
  {     
    average = average + altitude_avg[i];
  }
  // store the filter battery voltage
  altitude = average/5.0;*/
}

// read serial data transmitted from the overo and reply with suitable response
int printOvero()
{
  // print the compass data
  Serial.print('C');
  // format the compass data
  // less than 10
  if (compass_heading[1] == '.')
  {
    Serial.print('0');
    Serial.print('0'); 
  }
  // less than 100
  if (compass_heading[2] == '.')
  {
    Serial.print('0'); 
  }
  for(i=0; i<compass_heading_index; i++)
  {
    Serial.print(compass_heading[i],BYTE);
  }
  Serial.print(",V");
  Serial.print(bat_voltage,3);
  Serial.print(",A");
  Serial.print(altitude,3);
  Serial.print('\n');      
}
