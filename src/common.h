#pragma once
#include <Arduino.h>

//#define DEBUG

// BSFrance 2017 contact@bsrance.fr 
// LoR32u4II 868MHz or 915MHz (black board)
#define LORA_SS      8
#define LORA_RST     4
#define LORA_DI0     7
#define LORA_BAND    868E6
#define LORA_PABOOST false 

// battery
#define BATTERY_VMAX 4.2
#define BATTERY_VMIN 3.3
#define BATTERY_CALIB (4.2 / 1024.) //3.3 * 2 / 1024.

// radio
#define LORA_BOAT_ID            0x14
#define LORA_CLIENT_ADR         0x55
#define LORA_SERVER_ADR         0xFF

// remote timers
#define REMOTE_EXPIRES  250
#define REMOTE_INTERVAL 100

// esp webserver
#define ESP_BAUDRATE 250000
#define ESP_SERIAL Serial1

// radio communications
#define SIGNAL_MIN  0
#define SIGNAL_MAX  5
#define SIGNAL_LOWPASS 0.25
#define RF_TIMEOUT 50

// shunt
#define SHUNT_A  150
#define SHUNT_mV 75

// commands
enum Move : int8_t
{
    DIR_STOP = 4,           // stop
    DIR_POS = 1,            // move positive direction
    DIR_NEG = -1,           // move negative direction
    DIR_FORCE_POS = 2,      // move positive direction, ignore limit
    DIR_FORCE_NEG = -2,     // move negative direction, ignore limit
    NOOP = 0,               // do nothing
    SET_DEPTH = 9,          // set depth of anchor, calculate new link_length factor
    SET_ZERO = 10,          // set anchor zero
    SET_MAX = 11,           // set anchor max
    SET_COOLDOWN = 12,      // set direction change cooldown
    ERROR_DISABLED = 32,    // motor is disabled
    ERROR_COOL = 33,        // motor is in cooldown
    ERROR_LIMIT = 34,       // motor at limit
    UNKNOWN = 64            // value unknown
};

// Data packet
typedef struct __attribute((__packed__)) {
    uint8_t sender;
    uint16_t msg_id;
    float signal;
    float depth;
    //float current;
    //float voltage;
    Move thruster;
    Move windlass;
} RemotePacket;
