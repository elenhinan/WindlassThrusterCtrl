#pragma once
#include <Arduino.h>
#include "config.h"

class BatteryInfoClass {
    private:
        uint8_t _pin_vbat;
        uint8_t _pin_bat_mon;
        float _vmin;
        float _vmax;
        float _adcfactor;
        float _vbat_filtered;
        unsigned long _ts_prev;

    public:
        BatteryInfoClass();
        void begin();
        void update();
        void setPins(uint8_t vbat, uint8_t bat_chg_en) { _pin_vbat = vbat; _pin_bat_mon = bat_chg_en; }
        void setVoltages(float vmin, float vmax) { _vmin = vmin; _vmax = vmax; }
        void setCalibration(float factor) { _adcfactor = factor; }
        float getVoltage();
        float getPercentage();
};

extern BatteryInfoClass battery;