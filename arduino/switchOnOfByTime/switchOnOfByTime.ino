#include <EEPROM.h>
#include <Wire.h>
#include<stdio.h>
#include "RTClib.h"

#define ADDITIONAL_GROUND_PIN 2
#define RELAY_PIN 4
#define LED_PIN 13

RTC_DS1307 rtc;


#define SHOULD_BE_ON_ARRAY_SIZE ((24 * 60) / 8) + 1
byte shouldBeOnArray[SHOULD_BE_ON_ARRAY_SIZE];

#define WRITE_TO_EEPROM 0
#define SET_TIME 0

void setup () {
  Serial.begin(57600);
  pinMode(ADDITIONAL_GROUND_PIN, OUTPUT);
  digitalWrite(ADDITIONAL_GROUND_PIN, LOW);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  
  switchOff();

  Wire.begin();
  rtc.begin();

  if (SET_TIME) {
    Serial.println("SETTING TIME");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running - will be always OFF!");
  }
  else
  {
    Serial.println("RTC is running!");
  }

  initializeEeprom();
  readEeprom();
}

void initializeEeprom()
{
  if (!WRITE_TO_EEPROM)
  {
    Serial.println("WRITE_TO_EEPROM not set - skipping it!");
    return;
  }

  Serial.println("WRITE_TO_EEPROM set - initializing EEPROM!");
  
  setBits(shouldBeOnArray, 0, SHOULD_BE_ON_ARRAY_SIZE * 8, 0);

  setBits(shouldBeOnArray, 7 * 60 + 15, 7 * 60 + 45, 1);
  setBits(shouldBeOnArray, 19 * 60 + 0, 20 * 60 + 0, 1);
  setBits(shouldBeOnArray, 21 * 60 + 20, 22 * 60 + 16, 1);

  for (int i = 0; i < 24 * 60; ++i)
  {
    setBit(shouldBeOnArray, i, i % 2 == 1);
  }

  for (int i = 0; i < SHOULD_BE_ON_ARRAY_SIZE; ++i)
  {
    EEPROM.update(i, shouldBeOnArray[i]);
  }
}

void readEeprom()
{
  for (int i = 0; i < SHOULD_BE_ON_ARRAY_SIZE; ++i)
  {
    shouldBeOnArray[i] = EEPROM.read(i);
  }  
}

void setBits(byte array[], int fromIncl, int toExcl, bool value)
{
  for (int i = fromIncl; i < toExcl; ++i)
  {
    setBit(array, i, value);
  }
}

void setBit(byte array[], int bitIdx, bool value)
{
  int index = bitIdx / 8;
  int bitToSet = bitIdx % 8;

  if (value)
  {
    array[index] |= 1 << bitToSet;
  }
  else
  {
    array[index] &= ~(1 << bitToSet);
  }
}

bool isBitSet(byte array[], int bitIdx)
{
  int index = bitIdx / 8;
  int bitToIdx = bitIdx % 8;

  return (array[index] & (1 << bitToIdx)) > 0;
}

int getDayOffsetInMinutes()
{
  long nowInSecTillEpoch = rtc.now().unixtime();
  long dayOffsetSec = nowInSecTillEpoch % (24 * 3600L);

  char buffer[30];
  sprintf(buffer, "now(): %ld h=%ld, m=%ld, sec=%ld\n", nowInSecTillEpoch, dayOffsetSec / 3600, (dayOffsetSec % (3600)) / 60, dayOffsetSec % 60);
  Serial.print(buffer);

  return dayOffsetSec / 60;
}

void switchOff()
{
  Serial.println("switchOff");
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}


void switchOn()
{
  Serial.println("switchOn");
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void switchTo(bool value)
{
  if (value)
  {
    switchOn();
  }
  else
  {
    switchOff();
  }
}

bool shouldBeOnFor(int minute)
{
  return isBitSet(shouldBeOnArray, minute);
}


bool alarmLedState = false;
void loop() {
  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running - will be always OFF!");
    delay(100);
    alarmLedState = !alarmLedState;
    digitalWrite(LED_PIN, alarmLedState ? HIGH : LOW);
    return;
  }

  int dayOffsetInMinutes = getDayOffsetInMinutes();
  bool shouldBeOnNow = shouldBeOnFor(dayOffsetInMinutes);

  Serial.print("Should be on: ");
  Serial.println(shouldBeOnNow);

  switchTo(shouldBeOnNow);
  
  delay(10000);
}
