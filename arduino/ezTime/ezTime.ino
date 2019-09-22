#include <ezTime.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.begin("Internet domowy", "MzQ3OEM0");

  waitForSync();

  Serial.println("UTC: " + UTC.dateTime());
  
  Timezone NewZealand;
  NewZealand.setLocation("Pacific/Auckland");
  Serial.println("New Zealand time: " + NewZealand.dateTime());
}

void loop() { }
