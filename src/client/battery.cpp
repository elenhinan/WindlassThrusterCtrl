#include "battery.h"

BatteryInfoClass::BatteryInfoClass() :
    _pin_vbat(0),
    _vmin(3.6),
    _vmax(4.2),
    _adcfactor(1),
    _vbat_filtered(NAN)
{

}

void BatteryInfoClass::begin() {
    pinMode(_pin_vbat, INPUT);
    pinMode(_pin_bat_mon, OUTPUT);
    _ts_prev = millis();
    _vbat_filtered = analogRead(_pin_vbat) * _adcfactor;
}

void BatteryInfoClass::update() {
    unsigned long ts_now = millis(); // current sample time
    if (ts_now - _ts_prev > BATTERY_TS) {
        digitalWrite(_pin_bat_mon, HIGH);
        delay(1);
        float vbat = analogRead(_pin_vbat) * _adcfactor; // read current value
        digitalWrite(_pin_bat_mon, LOW);
        float alpha = max((ts_now - _ts_prev) * (1./BATTERY_TC), 1.0);
        _vbat_filtered = (1-alpha)*_vbat_filtered + alpha*vbat; // save low pass filtered voltage
        _ts_prev = ts_now;
    }
}

float BatteryInfoClass::getVoltage() {
    return _vbat_filtered;
}

float BatteryInfoClass::getPercentage() {
    float ratio = (_vbat_filtered - _vmin) / (_vmax - _vmin);
    return constrain(ratio,0.0,1.0);
}

BatteryInfoClass battery;