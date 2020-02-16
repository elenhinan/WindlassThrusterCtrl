#include "battery.h"

BatteryInfoClass::BatteryInfoClass() :
    _pin_vbat(0),
    _vmin(3.6),
    _vmax(4.2),
    _adcfactor(1)
{

}

void BatteryInfoClass::begin() {
    pinMode(_pin_vbat, INPUT);
}

float BatteryInfoClass::getVoltage() {
    return analogRead(_pin_vbat) * _adcfactor;
}

float BatteryInfoClass::getPercentage() {
    float ratio = (getVoltage() - _vmin) / (_vmax - _vmin);
    return constrain(ratio,0.0,1.0);
}

BatteryInfoClass battery;