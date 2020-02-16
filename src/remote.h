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
} RemotePacket;

class RemoteClass {
    private:
        RemotePacket _incomming;
        RemotePacket _outgoing;
        bool _incomming_valid;
        unsigned long _incomming_timestamp;
        int _msg_id;

        void _readPacket();
        void _writePacket();
    public:
        RemoteClass();
        void begin();
        bool recieve();
        bool transmit();
        void setThruster(Move state);
        Move getThruster();
        void setWindlass(Move state);
        Move getWindlass();
        void setDepth(float depth);
        float getDepth();
        float getSignal();
};

extern RemoteClass remote;