#include <VirtualWire.h>
#include <VirtualWire_Config.h>


//Connect the Receiver data pin to Arduino pin 11


int led = 13;
int counter = 0;
const int transmit_pin = 12;
const int receive_pin = 11;

byte message[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming messages
byte messageLength = VW_MAX_MESSAGE_LEN; // the size of the message

void setup() {
 Serial.begin(9600);
 Serial.println("Device is ready");
 pinMode(led, OUTPUT);
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  // Initialize the IO and ISR
 vw_setup(2000); // Bits per sec
 vw_rx_start(); // Start the receiver
}

void loop() {
  ++counter;
  if (vw_get_message(message, &messageLength)) {  // Non-blocking
    Serial.print("Received: ");
    for (int i = 0; i < messageLength; i++) {
     Serial.write(message[i]);
    }
   if((message[0]==111) && (message[1]==110) ) { // this is "on": "o" corresponds to 111, "n" corresponds to 110
    digitalWrite(13, HIGH);
   }
   else {
    digitalWrite(13, LOW);  
   }
   Serial.println();
  }
  delay(50);

  if (counter % 20 == 0)
  {
    Serial.println("pimg");
  }
}
