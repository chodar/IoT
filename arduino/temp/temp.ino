/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <OneWire.h>
#include <DallasTemperature.h>




const char* host = "184.106.153.149";
const char* ssid = "virginmedia2955462";
const char* password = "sdbcvqjp";

//const char* ssid = "AndroidAP";
//const char* password = "12345678";

#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


void setup() {
  Serial.begin(115200);
  delay(10);

  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

getSmoke();
  getTemperature();
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

}

int value = 0;

void loop() {
  delay(5000);
  ++value;
getTemperature();
}


String getSmoke()
{
  String result = String(analogRead(A0));
  Serial.print("Smoke level " + result);

  return result;
}

String getTemperature()
{
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  Serial.print("Temperature for Device 1 is: ");

  float temperature = sensors.getTempCByIndex(0);

  char buffer[10];
  String temperatureString = dtostrf(temperature, 4, 1, buffer);

  Serial.println(temperatureString); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire

  return temperatureString;
}


