#pragma once
#include <Arduino.h>

// BSFrance 2017 contact@bsrance.fr 
// LoR32u4II 868MHz or 915MHz (black board)
#define LORA_SS      8
#define LORA_RST     4
#define LORA_DI0     7
#define LORA_BAND    868E6
#define LORA_PABOOST false 

// radio
#define LORA_BOAT_ID            0x14
#define LORA_CLIENT_ADR         0x55
#define LORA_SERVER_ADR         0xFF

// commands
enum Move : int8_t
{
    DIR_STOP = 0,       // stop
    DIR_POS = 1,        // move positive direction
    DIR_NEG = -1,       // move negative direction
    ERROR_DISABLED,     // motor is disabled
    ERROR_COOL,         // motor is in cooldown
    ERROR_LIMIT,        // motor at limit
    SET_DEPTH,          // set depth of anchor, calculate new link_length factor
    SET_ZERO,           // set anchor zero
    NOOP,               // do nothing
    UNKNOWN             // value unknown
};

Move hwinput(uint8_t btn_neg, uint8_t btn_pos) {
    return (Move)(!digitalRead(btn_neg) - !digitalRead(btn_pos));
}