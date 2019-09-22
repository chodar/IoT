/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>


const char* host = "api.thingspeak.com";

//const char* ssid = "AndroidAP";
//const char* password = "12345678";

const char* ssids[] = { "3MobileWiFi-6CFB", "virginmedia2955462" };
const char* passwds[] = { "3dnjq548", "sdbcvqjp" };
const int ssidsNum = 2;

const char* apiKeys[] = {"YGUSONH3PHNZKW3Q", "9VDTPW5PDGZXQS8Y"};
#define CURRENT_DEVICE 0

#define LED1 D0
#define LED2 D4

#define MOTION_PIN D6

#define ITERATION_DELAY_MS 100



void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  pinMode(MOTION_PIN, INPUT);
  
  setLedState(LED1, false);
  setLedState(LED2, false);
  delay(10);

  bool isConnected = connectToAp();

  if (!isConnected)
  {
    Serial.println("Can't connec to to WiFi - restarting....");
    delay(1000);
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

int iterationNum = 0;

void loop() {
  Serial.println(String("Starting iteration: ") + String(iterationNum) + String(", uptime sec: ") + (millis()/1000));

  setLedState(LED2, motionDetected());
  
  Serial.println("All good, sleeping before next iteration");
  Serial.println();
  delay(ITERATION_DELAY_MS);
  ++iterationNum;
}

bool motionDetected()
{
  return digitalRead(MOTION_PIN);
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


