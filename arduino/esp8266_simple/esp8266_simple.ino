/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define USE_DHT22 1

#include <ESP8266WiFi.h>

#ifdef USE_DHT22
#include <OneWire.h>
#include <DallasTemperature.h>
#else
#include "DHTesp.h" // https://github.com/beegee-tokyo/DHTesp
#endif

const char* host = "api.thingspeak.com";


const char* ssids[] = { "HUAWEI-E5330-6EF0", "pawelc_hotspot" };
const char* passwds[] = { "q6a27y82", "83110511433" };
const int ssidsNum = 2;

const char* apiKeys[] = {"YGUSONH3PHNZKW3Q", "9VDTPW5PDGZXQS8Y"};
#define CURRENT_DEVICE 1

#define ONE_WIRE_BUS D5
#define RESET_SIGNAL_PIN D6
#define LED1 D0
#define LED2 D4

#define ITERATION_DELAY_MS 60*1000
#define REBOOT_EVERY_MS 60*60*1000

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  setLedState(LED1, false);
  setLedState(LED2, false);
  delay(10);

  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  getSmoke();
  getTemperature();
  // We start by connecting to a WiFi network
  Serial.println();
  
  bool isConnected = connectToAp();

  if (!isConnected)
  {
    Serial.println("Can't connec to to WiFi - restarting....");
    delay(1000);
    restart();
  }

}

bool connectToAp()
{
  int networkNum = WiFi.scanNetworks();

  for (int i = 0; i < ssidsNum; ++i)
  {
    for (int networkIdx = 0; networkIdx < networkNum; ++networkIdx)
    {
      String ssid = String(WiFi.SSID(networkIdx));
      if (ssid == String(ssids[i]))
      {
        Serial.println("Network found, ssid: " + ssid);
        WiFi.begin(ssids[i], passwds[i]);

        int iterations = 0;
        while (WiFi.status() != WL_CONNECTED) {
          blink(LED1, 1, 500);
          blink(LED2, 1, 500);
          Serial.print(".");
          ++iterations;

          if (iterations > 100)
          {
            Serial.println("Timeout when waiting for connecting to wifi!");
            blink(LED2, 3, 500);
            return false;
          }
        }
      
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
      }
    }    
  }

  return false;
}

int iterationNum = 0;

void setLedState(int ledPin, bool enabled)
{
  digitalWrite(ledPin, enabled? LOW : HIGH); 
}

void blink(int led, int times, int delayMs)
{
  for (int i = 0; i < times; ++i)
  {
    if (i != 0)
    {
      delay(delayMs);
    }
    
    setLedState(led, true);
    delay(delayMs);  
    setLedState(led, false);
  }
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void restart()
{
  Serial.println("Trying to restart!");
  blink(LED1, 5, 200);
  
  pinMode(RESET_SIGNAL_PIN, OUTPUT);
  digitalWrite(RESET_SIGNAL_PIN, LOW);
  delay(100);
  digitalWrite(RESET_SIGNAL_PIN, HIGH);
  Serial.println("Restart FAILED!");
  resetFunc();  //call reset
  Serial.println("Restart FAILED even more!");
}

void loop() {
  Serial.println(String("Starting iteration: ") + String(iterationNum) + String(", uptime sec: ") + (millis()/1000));

  String url = "/update?field1=" + getTemperature() + "&field2=" + getSmoke() + "&field8=" + String(iterationNum) + "&api_key=" + apiKeys[CURRENT_DEVICE];
  
  String response;
  bool succeeded = issueGetRequestWithRetries(3, 1000, url, host, 80, response);
  Serial.println("Response: " + response + ", succeeded: " + String(succeeded));

  if (millis() >= REBOOT_EVERY_MS || !succeeded)
  {
    restart();
  }

  Serial.println("All good, sleeping before next iteration");
  Serial.println();
  delay(ITERATION_DELAY_MS);
  ++iterationNum;
}

bool issueGetRequestWithRetries(int numRetries, int backoffMs, String url, String host, int port, String & response)
{
  for (int i = 0; i <= numRetries; ++i)
  {
    bool succeeded = issueGetRequest(url, host, port, response);

    if (succeeded)
    {
      return true;
    }

    Serial.println("Error when issuing a request, attempt: " + String(i));
    if (i < numRetries)
    {
      blink(LED2, 1, backoffMs);
    }
  }

  Serial.println("All retries exhausted - giving up!");
  return false;
}

bool issueGetRequest(String url, String host, int port, String & response)
{
  setLedState(LED1, true); 
  Serial.println("connecting to " + host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  if (!client.connect(host.c_str(), port)) {
    Serial.println("connection failed");
    blink(LED2, 3, 300);
    return false;
  }

  Serial.println("Requesting URL: " + url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n");
  client.print(String("Host: ") + host + "\r\n\r\n");
  delay(1000);

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println("RESPONSE line: " + line);
    response += line;
  }

  Serial.println("closing connection");

  setLedState(LED1, false);
  return true;
}





String getSmoke()
{
  String result = String(analogRead(A0));
  Serial.println("Smoke level " + result);

  return result;
}

String getHumidity()
{
  float humidity = dht.getHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return "0";
  }

  
  String result = String(humidity);
  Serial.println("Humidity level " + result + "%");

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


