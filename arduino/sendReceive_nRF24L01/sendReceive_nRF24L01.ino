
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

byte masterAddress[6] = "mst01";

// Used to control whether this node is sending or receiving
bool role = 1;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  
  radio.begin();
  radio.openReadingPipe(1,masterAddress);
  
  // Start the radio listening for data
  radio.startListening();
}

void loop() {
      
    if( radio.available()){
                       
      float value;
      while (radio.available()) {                                   // While there is data ready
        radio.read( &value, sizeof(float) );             // Get the payload
      }
     
      Serial.print(F("Received "));
      Serial.println(value);  
   }

} // Loop

