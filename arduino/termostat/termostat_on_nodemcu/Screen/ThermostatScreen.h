#ifndef ThermostatScreen_h
#define ThermostatScreen_h

#include "Screen.h"

class ThermostatScreen
{
public:
ThermostatScreen()
{
        _screen.GotoXY(0,0);
        _screen.OutputString("Welcome :)");
}

void SetTemperature(double current, double target)
{
    _screen.GotoXY(0, 2);
    _screen.OutputString("            ");
    _screen.GotoXY(0, 2);

    String toOutput = String(current) + String("/")  + String(target) + String("C");

    char buffer[13];
    toOutput.toCharArray(buffer, 13);
    _screen.OutputString(buffer);
}

void SetPowerState(bool isOn)
{
    _screen.GotoXY(0, 3);
    _screen.OutputString("            ");
    _screen.GotoXY(0, 3);

    String toOutput = String("Power: ") + (isOn ? String("ON") : String("OFF"));

    char buffer[12];
    toOutput.toCharArray(buffer, 12);
    _screen.OutputString(buffer);
}
void SetTime(DateTime now)
{
    _screen.GotoXY(0, 1);
    _screen.OutputString("            ");
    _screen.GotoXY(0, 1);

    String nowAsString = String("") + now.hour() + ":" + now.minute() + ":" + now.second();

    char buffer[12];
    nowAsString.toCharArray(buffer, 12);
    _screen.OutputString(buffer);
}

private:
Screen _screen;
};

#endif