#include "NewSoftSerial.h"

// compass transmit pin
#define COMPASS_TX 4
// compass receive pin
#define COMPASS_RX 5
// compass baudrate
#define COMPASS_BAUDRATE 19200
// overo baudrate
#define OVERO_BAUDRATE 115200

// sofrware serial compass object
NewSoftSerial compass(COMPASS_RX,COMPASS_TX);

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
 char c;
 // read the compass and print to overo
 if (compass.available() > 0)
 {
   c = compass.read();
   Serial.print(c);
 }
 // read from the overo and print to the compass
 if (Serial.available() > 0)
 {
   c = Serial.read();
   compass.print(c);
 }
}
