#include <SoftwareSerial.h>

SoftwareSerial portOne(6, 7);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // Start each software serial port
  portOne.begin(9600);
}

void loop() {
  portOne.println("ello from portOne on com3");

  Serial.println("printing from port");
  while (portOne.available() > 0) {
    char inByte = portOne.read();
    Serial.write(inByte);
  }

  // blank line to separate data from the two ports:
  Serial.println("prnting from port done");

  
  // blank line to separate data from the two ports:
  Serial.println("next loop");
  delay(5000);
}
