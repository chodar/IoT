#include <stdlib.h>
#include <SoftwareSerial.h>

SoftwareSerial esp01(7, 8);

String GET;
String Thingspeak_KEY = "2AJPJJ64UFTN0Q0E"; //Tu wpisz swój Thingspeak KEY.
String NazwaSieci = "virginmedia2955462"; //Tu podaj nazwe swojej sieci.
String HasloSieci = "sdbcvqjp"; //Tu hasło do sieci.

void setup()
{
  Serial.begin(9600);
  Serial.println("Send over WiFi");

  esp01.begin(9600);
  sendCommand("AT");
  sendCommand("AT+CIPMUX=1");
  connect();
  Serial.println("Connecting");
}

void loop() {
  Serial.println("starting loop");
  sendCommand("AT");
  return;
  char buffer[10];
  char buffer1[10];
  float Btemp = 12.4, Bhumi = 80.88;

  String Temp = dtostrf(Btemp, 4, 1, buffer);
  updateTemp(Temp); //wysylam dane
  Serial.println("turn done");
}
void updateTemp(String Temp) {
  GET = "GET /update?key=";
  GET += Thingspeak_KEY;
  GET += "&field1=";
  String command = GET;
  command += String(Temp);
  command += "\n\r\n\r";
  sendCommand("AT+CIPSTART=4, \"TCP\",\"184.106.153.149\",80");
  sendCommand("AT+CIPSEND=4,46");
  sendCommand(command);
  sendCommand("AT+CIPCLOSE");
}

void connect() {
  sendCommand("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"" + NazwaSieci + "\", \"" + HasloSieci + "\"";
  sendCommand(cmd);
}


/*
* Funkcja wysyłająca komendy do modułu ESP01
* Parametry:
* command - komenda do wysłania
* timeout - czas oczekiwania na odpowiedź
*/
String sendCommand(String command)
{
  const int timeout = 3000;
  String response = "r: ";

  esp01.print(command); // wysłanie polecenia do ESP01

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp01.available()) //jeśli są jakieś dane z modułu, wtedy następuje ich odczyt
    {
      char c = esp01.read(); // odczyt kolejnego znaku
      response += c;
    }
  }
  Serial.println(response);
  return response;
}

