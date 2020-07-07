#pragma once
#include <Arduino.h>
#include <LoRa.h>
#include "common.h"

typedef struct {
    uint8_t sender;
    uint16_t msg_id;
    float signal;
    float depth;
    Move thruster;
    Move windlass;
} RadioPacket;

class RadioClass {
    private:
        RadioPacket _incomming;
        RadioPacket _outgoing;
        bool _changed;
        bool _incomming_valid;
        unsigned long _incomming_timestamp;
        unsigned long _outgoing_timestamp;
        int _msg_id;
        float _sig_min;
        float _sig_max;
        float _lowpass_signal;
        void _readPacket();
        void _writePacket();
    public:
        RadioClass();
        void begin();
        bool recieve();
        bool transmit();
        bool isValid();
        void setThruster(Move state);
        Move getThruster();
        void setWindlass(Move state);
        Move getWindlass();
        void setDepth(float depth);
        float getDepth();
        float getSignal();
        void setSignalRange(float sigmin, float sigmax) { _sig_min = sigmin; _sig_max = sigmax; }
        float getSNR();
};

extern RadioClass radio;