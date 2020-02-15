#include "thruster.h"

Thruster::Thruster(const char* name, uint8_t starboard_out, uint8_t port_out, uint8_t starboard_in, uint8_t port_in) :
    Motor(name, starboard_out, port_out, starboard_in, port_in)
{
    
}

bool Thruster::begin() {
    return Motor::begin();
}