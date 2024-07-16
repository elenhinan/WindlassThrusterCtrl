#pragma once
#include <Arduino.h>
#include <LoRa.h>
#include "common.h"
#include "remote.h"

class RadioClass : public RemoteClass {
    private:
        float _lowpass_signal;
        void _readPacket();
        void _writePacket();
    public:
        RadioClass();
        void begin();
        bool recieve(unsigned long timeout=0);
        float getSignal();
        float getSNR();
};

#ifdef DEBUG
void _onTxDone();
#endif

extern RadioClass radio;