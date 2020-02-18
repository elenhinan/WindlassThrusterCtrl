#pragma once
#include <Arduino.h>
#include "common.h"

class Motor {
    protected:
        char _name[12];
        uint8_t _pin_positive_out;
        uint8_t _pin_negative_out;
        uint8_t _pin_positive_in;
        uint8_t _pin_negative_in;
        bool _enabled;
        unsigned long _cooldown;
        unsigned long _timeout;
        Move _current_direction;
        Move _cooldown_direction;
        Move _last_input_direction;
        unsigned long _cooldown_timestamp;
        unsigned long _last_timestamp;
        int16_t _chain_counter;
        virtual Move _hwinput();
        virtual void _output(Move direction);

    public:
        Motor(const char* name, uint8_t pin_positive_out, uint8_t pin_negative_out, uint8_t pin_positive_in, uint8_t pin_negative_in);
        bool begin();
        void input(Move direction);
        void update();
        bool enable();
        void disable();
        void setTimeout(unsigned long timeout) { _timeout = timeout; }
        void setCooldown(unsigned long cooldown) { _cooldown = cooldown; }
        Move getState() { return _current_direction; }
};