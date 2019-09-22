#include <LowPower.h>
#include <stdlib.h>

#include <SoftwareSerial.h>

#define TRANSITOR_BASE_PIN 4

#define SSID "virginmedia2955462"
#define PASS "sdbcvqjp"
#define IP "translate.ubidots.com"
String GET = "GET /api/postvalue/?token=bovgOfnIczENvkSDiVxxwEf5x71Xgg&variable=561a588076254279d6027374&value=";
SoftwareSerial esp(7, 8); // RX, TX


void setup()
{
  Serial.begin(9600);
  
  digitalWrite(TRANSITOR_BASE_PIN, HIGH);
  pinMode(TRANSITOR_BASE_PIN, OUTPUT);

  esp.begin(9600);
  sendCommand(esp, "AT");
  
}

void loop(){
  digitalWrite(TRANSITOR_BASE_PIN, HIGH);
  connectWiFi();
  save_value(String(analogRead(A0)));

  disconnectWiFi();
  digitalWrite(TRANSITOR_BASE_PIN, LOW);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  //delay(1000);

  
}

void save_value(String value){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  sendCommand(esp, cmd);
  
  cmd = GET;
  cmd += value;
  cmd += "\r\n";

  sendCommand(esp, String("AT+CIPSEND=") + cmd.length());
  sendCommand(esp, cmd);
  sendCommand(esp, "AT+CIPCLOSE");
}

String sendCommand(SoftwareSerial & receiver, String command)
{
  const int timeout = 1000;
  String response = "response: ";

  Serial.println("command: " + command);
  receiver.print(command + "\r\n");

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (receiver.available())
    {
      char c = receiver.read(); 
      response += c;
    }
  }
  Serial.println(response);
  return response;
}

boolean connectWiFi(){
  sendCommand(esp, "AT+CWMODE=1");
  
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  sendCommand(esp, cmd);
  
  while (sendCommand(esp, "AT+CWJAP?").indexOf("busy") != -1);
 
}

void disconnectWiFi()
{
  sendCommand(esp, "AT+CWQAP");
}

