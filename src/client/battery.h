#pragma once
#include <Arduino.h>
#include "config.h"

class BatteryInfoClass {
    private:
        uint8_t _pin_vbat;
        float _vmin;
        float _vmax;
        float _adcfactor;

    public:
        BatteryInfoClass();
        void begin();
        void setPins(uint8_t vbat) { _pin_vbat = vbat; }
        void setVoltages(float vmin, float vmax) { _vmin = vmin; _vmax = vmax; }
        void setCalibration(float factor) { _adcfactor = factor; }
        float getVoltage();
        float getPercentage();
};

extern BatteryInfoClass battery;