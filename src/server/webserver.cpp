#include "webserver.h"

PacketSerial webPacket;

WebServerClass::WebServerClass()
{
}

void WebServerClass::begin() {
    webPacket.begin(ESP_BAUDRATE, ESP_SERIAL);
    webPacket.setPacketHandler(&onPacketReceived, this);
}

void WebServerClass::_readPacket(const uint8_t* buffer, size_t size) {
    if (size == sizeof(RemotePacket)) {
        memcpy(&_incomming, buffer, size);
        _incomming_timestamp = millis();
        _incomming_updated = true;
        _valid = true;
    }
}

void WebServerClass::_writePacket() {
    _outgoing.msg_id++;
    webPacket.send((uint8_t*)&_outgoing, sizeof(RemotePacket));
}

bool WebServerClass::recieve(unsigned long timeout) {
    webPacket.update();
    return _incomming_updated;
};

WebServerClass webserver;