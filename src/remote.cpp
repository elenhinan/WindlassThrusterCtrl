#include "remote.h"

RemoteCtrl::RemoteCtrl() {
    _outgoing.sender = 'T';
    _outgoing.msg_id = 0;
}

void RemoteCtrl::begin() {
    Serial.println("LoRa Duplex");
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);// set CS, reset, IRQ pin
    
    if (!LoRa.begin(LORA_BAND, LORA_PABOOST)) {             // initialize ratio at 915 MHz
        Serial.println("LoRa init failed. Check your connections.");
        while (true);                       // if failed, do nothing
    }
    LoRa.setSyncWord(LORA_BOAT_ID);
    LoRa.setTxPower(10, false);
    LoRa.enableCrc();
    Serial.println("LoRa init succeeded.");
}

void RemoteCtrl::_readPacket() {
    uint8_t* pkg_ptr = (uint8_t*)&_incomming;

    if (LoRa.parsePacket() == sizeof(RemotePacket)) {
        for (uint8_t i=0;i<sizeof(RemotePacket);i++) {
            *pkg_ptr++ = (uint8_t)LoRa.read();
        }
        _outgoing.signal = LoRa.packetSnr();
        _incomming_valid = true;
    } else {
        _incomming_valid = false;
    }
}

void RemoteCtrl::_writePacket() {
    uint8_t* pkg_ptr = (uint8_t*)&_outgoing;
    LoRa.beginPacket();
    LoRa.write(pkg_ptr, sizeof(RemotePacket));
    LoRa.endPacket();
    _outgoing.msg_id++;
}

bool RemoteCtrl::recieve() {
    _readPacket();
    return _incomming_valid;
};

bool RemoteCtrl::transmit() {
    _writePacket();
    return true;
}

void RemoteCtrl::setThruster(Move state) {
    _outgoing.thruster = state;
}

Move RemoteCtrl::getThruster() {
    return _incomming_valid ? _incomming.thruster : UNKNOWN;
}

void RemoteCtrl::setWindlass(Move state) {
    _outgoing.windlass = state;
}
Move RemoteCtrl::getWindlass() {
    return _incomming_valid ? _incomming.windlass : UNKNOWN;
}

void RemoteCtrl::setDepth(float depth) {
    _outgoing.depth = depth ;
}

float RemoteCtrl::getDepth() {
    return _incomming_valid ? _incomming.depth : NAN;
}

float RemoteCtrl::getSignal() {
    return _incomming_valid ? _incomming.signal : NAN;
}