
#include "c:\private\arduino\mylib\world.h"
#include "clock.h"
#include "Screen/ThermostatScreen.h"

#define INTERVAL_MS 200

#define TEMPERATUR_PIN D6
#define MAIN_CONTROL D0
#define LED 13

#define START_MAIN_IF_LOWER 30.0
#define STOP_MAIN_IF_HIGHER 31.0

boolean isMainOn = false;


ThermostatScreen screen;

World * world = NULL;
 

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize digital pin 13 as an output.
  //pinMode(LED, OUTPUT);
  pinMode(MAIN_CONTROL, OUTPUT);
  
    // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  world = new WorldDht22(TEMPERATUR_PIN);
  world->printState();

  digitalWrite(MAIN_CONTROL, LOW);
}

Clock clock;
int i  = 0;
// the loop function runs over and over again forever
void loop() {

  float value = readTemperature();
  float target = getTargetTemperature();
  
  screen.SetTemperature(value, target);
  
  if (value < target)
  {
     turnMainOn();
  }
  else if (value > target)
  {
    turnMainOff();
  }
  screen.SetTime(clock.Now());

  delay(INTERVAL_MS);  
}

float getTargetTemperature()
{
  return 28.3;
}

float readTemperature()
{
  world->printState();
  return world->getTemperature();
}


void turnMainOff()
{
  digitalWrite(MAIN_CONTROL, HIGH);
  digitalWrite(LED, LOW);
  screen.SetPowerState(false);
}

void turnMainOn()
{
  digitalWrite(MAIN_CONTROL, LOW);
  digitalWrite(LED, HIGH);
  screen.SetPowerState(true);
}

