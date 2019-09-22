#include <SPI.h>
#include "RF24.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Pins
#define ONE_WIRE_BUS 2
#define GROUND 3

#define MEASURE_NAME "temperature"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#define REPORT_INTERVAL_MS 1000

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

byte myAddress[6] = "ard02";
byte masterAddress[6] = "mst01";

void setup() {
  Serial.begin(115200);
  Serial.println(F("ReportSensoToRadio2.4g"));
  
  pinMode(GROUND, OUTPUT);
  digitalWrite(GROUND, LOW);
  
  radio.begin();
  radio.setChannel(1);
  radio.openWritingPipe(masterAddress);
  
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  
  radio.printDetails();
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  Serial.print("Temperature for Device 1 is: ");
  
  float temperature = sensors.getTempCByIndex(0);
  Serial.println(temperature); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  sendMeasure(temperature);
  delay(REPORT_INTERVAL_MS + random(0, REPORT_INTERVAL_MS));  
}

void sendMeasure(float value)
{
    radio.powerUp();
    Serial.println(F("Now sending"));
     if (!radio.write(&value, sizeof(float) )){
       Serial.println(F("failed"));
     }
     
     radio.powerDown();
}
  
