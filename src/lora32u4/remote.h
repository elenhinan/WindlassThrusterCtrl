#pragma once
#include <Arduino.h>
#include "common.h"

class RemoteClass {
    protected:
        RemotePacket _incomming;
        RemotePacket _outgoing;
        bool _changed;
        bool _incomming_updated;
        unsigned long _incomming_timestamp;
        unsigned long _outgoing_timestamp;
        bool _valid;
        int _msg_id;
        virtual void _writePacket() = 0;
    public:
        RemoteClass();
        virtual void begin() = 0;
        virtual bool recieve(unsigned long timeout=0) = 0;
        bool transmit(bool force=false);
        bool isValid();
        void setThruster(Move state);
        Move getThruster();
        void setWindlass(Move state);
        Move getWindlass();
        void setDepth(float depth);
        float getDepth();
};