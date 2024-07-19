#include "radiolink.h"

PacketSerial webPacket;
uint8_t ledpower=0;
int lastsize = 0;
JsonDocument doc;

LinkClass::LinkClass()
{
}

void LinkClass::begin() {
    webPacket.begin(ESP_BAUDRATE, Serial);
    webPacket.setPacketHandler(&_onPacketReceived, this);
}

void LinkClass::_readPacket(const uint8_t* buffer, size_t size) {
    if (size == sizeof(RemotePacket)) {
        memcpy(&_incomming, buffer, size);
        _incomming_timestamp = millis();
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    //char hex[3];
    //for (int i = 0; i < size; i++) {
    //    sprintf(hex, "%02x", buffer[i]);
    //    doc["data"][i]=hex;
    //}
}

void LinkClass::_toJson() {
    doc["windlass"] = _incomming.windlass;
    doc["thruster"] = _incomming.thruster;
    doc["depth"] = _incomming.depth;
    doc["age"] = millis() - _incomming_timestamp;

}

void LinkClass::getJson(String &buffer) {
    _toJson(); 
    serializeJson(doc, buffer);
}

void LinkClass::_writePacket() {
    _outgoing.msg_id++;
    webPacket.send((uint8_t*)&_outgoing, sizeof(RemotePacket));
}

void LinkClass::update() {
    webPacket.update();
};

LinkClass RadioLink;