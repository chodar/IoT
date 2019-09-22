#include <OneWire.h>
#include <DallasTemperature.h>
#include <VirtualWire.h>
#include <VirtualWire_Config.h>

// Pins
#define ONE_WIRE_BUS 2
#define POWER 3
#define TRANSIMIT_PIN 12

#define DEVICE_NAME "arduino1"
#define MEASURE_NAME "temperature"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#define REPORT_INTERVAL_MS 1000

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  pinMode(POWER, OUTPUT);

  vw_set_tx_pin(TRANSIMIT_PIN);
  vw_setup(2000); // Bits per sec
  
  digitalWrite(POWER, HIGH);
  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
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
  sendMeasure(DEVICE_NAME, MEASURE_NAME, temperature);
  delay(REPORT_INTERVAL_MS + random(0, REPORT_INTERVAL_MS));  
}


void sendMeasure(char* device, char* measureName, float value)
{
 char buffer[35];
 char valueAsChars[15];
 
 dtostrf(value, 4, 2, valueAsChars);
 sprintf(buffer,"%s;%s;%s", device,measureName,valueAsChars);
 send(buffer);
}

 void send (char *message)
 {
    Serial.print("Sending over radio: ");
    Serial.println(message);
    vw_send((uint8_t *)message, strlen(message));
    vw_wait_tx(); // Wait until the whole message is gone
 }
