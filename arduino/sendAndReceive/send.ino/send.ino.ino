#include <VirtualWire.h>
#include <VirtualWire_Config.h>

// Watch video here: https://www.youtube.com/watch?v=8VoDT6ae768

// connect transmitter data pin to Arduino pin 12

const int transmit_pin = 12;
const int receive_pin = 11;
int result;

void setup()
 {
 // Initialize the IO and ISR
  Serial.begin(9600);
    vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  pinMode(13, OUTPUT);
  vw_setup(2000); // Bits per sec
 }

void loop()
{  
  send("on");
  Serial.println("on");
  digitalWrite(13, HIGH);
  delay(1000);
  
  send("off");
  Serial.println("off");
  digitalWrite(13, LOW);
  delay(1000);
}

 void send (char *message)
 {
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx(); // Wait until the whole message is gone
 }
