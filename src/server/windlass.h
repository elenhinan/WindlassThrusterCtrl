#pragma once
#include <INA219.h>
#include "motor.h"

class Windlass : public Motor{
    private:
        uint8_t _pin_chain_encA;
        uint8_t _pin_chain_encB;
        int16_t _chain_counter;
        int16_t _chain_counter_max;
        float _chain_length;
        float _meter_per_pulse;
        float _current;
        float _voltage;
        bool _chain_encA_prev;
        bool _chain_encB_prev;
        bool _force_move;
        bool _quadrature;
        bool _checkLimits(Move direction);
        void _output(Move direction);
        void _decodeCounter();
        Move _hwinput();

    public:
        Windlass(const char* name, uint8_t pin_up_out, uint8_t pin_down_out, uint8_t pin_up_in, uint8_t pin_down_in, uint8_t pin_chain_encA, uint8_t pin_chain_encB);
        bool begin();
        void update();
        float getChain() { return float(_chain_counter) * _meter_per_pulse; }
        void setChain(float meters) { _chain_counter = meters / _meter_per_pulse; }
        float getMeterPerPulse() { return _meter_per_pulse; }
        void setMeterPerPulse(float meters) { _meter_per_pulse = meters; _chain_counter_max = floor(_chain_length / _meter_per_pulse); }
        float getChainLength() { return _chain_length ; }
        void setChainLength(float length) { _chain_length = length; _chain_counter_max = floor(_chain_length / _meter_per_pulse); }
};