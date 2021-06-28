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
        bool _incomming_updated;
        unsigned long _incomming_timestamp;
        unsigned long _outgoing_timestamp;
        int _msg_id;
        float _lowpass_signal;
        void _readPacket();
        void _writePacket();
    public:
        RadioClass();
        void begin();
        bool recieve(unsigned long timeout=0);
        bool transmit(bool force=false);
        bool isValid();
        void setThruster(Move state);
        Move getThruster();
        void setWindlass(Move state);
        Move getWindlass();
        void setDepth(float depth);
        float getDepth();
        float getSignal();
        float getSNR();
};

extern RadioClass radio;