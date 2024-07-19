
#pragma once
#include <Arduino.h>
#include <PacketSerial.h>
#include "common.h"
#include "lora32u4/remote.h"

class WebServerClass : public RemoteClass {
    private:
        void _readPacket(const uint8_t* buffer, size_t size);
        void _writePacket();
        static void onPacketReceived(const void* sender, const uint8_t* buffer, size_t size) {
            ((WebServerClass*)sender)->_readPacket(buffer, size);
        }
    public:
        WebServerClass();
        void begin();
        bool recieve(unsigned long timeout=0);
};

extern WebServerClass webserver;