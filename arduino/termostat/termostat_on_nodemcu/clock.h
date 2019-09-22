#ifndef MyClock_h
#define MyClock_h

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h" //https://github.com/adafruit/RTClib/archive/master.zip



class Clock
{
public:
Clock()
{
    if (! _rtc.begin()) 
    {
        Serial.println("Couldn't find RTC");
    }
}

DateTime Now()
{
        return _rtc.now();
}

private:
RTC_DS3231 _rtc;

};

#endif
