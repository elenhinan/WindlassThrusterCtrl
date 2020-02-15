#pragma once
#include "motor.h"

class Thruster : public Motor {
    
    public:
        Thruster(const char* name, uint8_t starboard_out, uint8_t port_out, uint8_t starboard_in, uint8_t port_in);
        bool begin();
};