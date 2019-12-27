#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

// https://github.com/ropg/ezTime
#include <ezTime.h>


#include "C:\private\IoT\arduino\mylib\world.h"

const char* host = "mobile.events.data.microsoft.com";

const char* ssids[] = { "Internet domowy", "Internet_Domowy_3478C4", "PLAY-ONLINE-F766", "pawelc_hotspot", "MSFTGUEST" };
const char* passwds[] = { "MzQ3OEM0", "MzQ3OEM0", "0MTN30DG", "83110511433", NULL };
const int ssidsNum = 5;

const char* devices[] = { "salon", "sypialnia_rodzicow", "sypialnia_Tomka", "sypialnia_Julka_i_Ignasia"};
const char * deviceName = devices[1];
#define RESET_SIGNAL_PIN 6
#define LED1 1
#define LED2 1

#define ITERATION_DELAY_MS 10*1000
#define REBOOT_EVERY_MS 60*60*1000
#define ONE_WIRE_BUS 4

World * world = NULL;


void setup() {
  Serial.begin(115200);

  world = new WorldDht22(ONE_WIRE_BUS);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  setLedState(LED1, false);
  setLedState(LED2, false);
  delay(10);

  world->printState();
  
  // We start by connecting to a WiFi network
  Serial.println();
  
  bool isConnected = connectToAp();

  if (!isConnected)
  {
    Serial.println("Can't connec to to WiFi - restarting....");
    delay(1000);
    restart();
  }

  waitForSync();
}

String getMacAsString()
{
  byte mac[6];
  WiFi.macAddress(mac);
  char macChars[18];
  sprintf(macChars, "%2X:%2X:%2X:%2X:%2X:%2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return String(macChars);
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

        
        WiFi.begin(ssid.c_str(), passwds[i]);

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
        Serial.println("Mac: " + getMacAsString());
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

String getFormatedTime()
{
  // 2019-12-26T19:54:30.835000Z
  return UTC.dateTime("Y-m-d\\TH:i:s.v000\\Z");
}

void loop() {
 events();
 
 Serial.println(String("Starting iteration: ") + String(iterationNum) + String(", uptime sec: ") + (millis()/1000) + String(", time: ") + getFormatedTime() );

  world->printState();
  //String url = "/update?field1=" + String(world->getTemperature()) + "&field2=" + String(world->getSmoke()) + "&field3=" + String(world->getHumidity()) + "&field8=" + String(iterationNum) + "&api_key=" + apiKeys[CURRENT_DEVICE];

  String url = "/OneCollector/1.0/t.js?qsp=True&apikey=4afa09bcb0ff4010abdb9e973c6a6eb1-beead718-7b4d-40f9-88fc-4c455a487959-7330&name=%22sensor_reading%22&iKey=%22o:4afa09bcb0ff4010abdb9e973c6a6eb1%22&*temperature=" + String(world->getTemperature()) + "&*humidity=" + String(world->getHumidity()) + "&*location=%22" + deviceName + + "%22&*device_id=%22" + getMacAsString() + "%22&time=%22" + getFormatedTime() + "%22";
  
  String response;
  bool succeeded = issueGetRequestWithRetries(3, 1000, url, host, 443, response);
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
  WiFiClientSecure  client;
  
  if (!client.connect(host.c_str(), port)) {
    Serial.println("connection failed");
    blink(LED2, 3, 300);
    return false;
  }

  

  String requestText = String("GET https://") + host + url + " HTTP/1.1\r\n";
  requestText += String("Host: ") + host + "\r\n\r\n";

  Serial.println("Request text: ");
  Serial.println(requestText);
  
  // This will send the request to the server
  client.print(requestText);
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
