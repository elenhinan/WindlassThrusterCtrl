#pragma once
#include <Arduino.h>
#include <PacketSerial.h>
#include <ArduinoJson.h>
#include "common.h"

class LinkClass {
    private:
        RemotePacket _incomming;
        RemotePacket _outgoing;
        void _readPacket(const uint8_t* buffer, size_t size);
        void _writePacket();
        void _toJson();
        void _fromJson();
        static void _onPacketReceived(const void* sender, const uint8_t* buffer, size_t size) {
            ((LinkClass*)sender)->_readPacket(buffer, size);
        }
    public:
        LinkClass();
        void begin();
        void update();
        void getJson(String &buffer);

};

extern LinkClass RadioLink;