#include "remote.h"

RemoteClass::RemoteClass() :
    _sig_min(0),
    _sig_max(0),
    _lowpass_signal(0)
{
    _outgoing.sender = 'T';
    _outgoing.msg_id = 0;
}

void RemoteClass::begin() {
    //Serial.println("LoRa Duplex");
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);// set CS, reset, IRQ pin
    
    if (!LoRa.begin(LORA_BAND)) {             // initialize ratio at 915 MHz
        //Serial.println("LoRa init failed. Check your connections.");
        while (true);                       // if failed, do nothing
    }
    LoRa.setSyncWord(LORA_BOAT_ID);
    LoRa.setTxPower(7, 1);
    LoRa.setSignalBandwidth(62.5E3);
    LoRa.setCodingRate4(8);
    LoRa.enableCrc();
    //Serial.println("LoRa init succeeded.");
}

void RemoteClass::_readPacket() {
    uint8_t* pkg_ptr = (uint8_t*)&_incomming;

    if (LoRa.parsePacket() == sizeof(RemotePacket)) {
        for (uint8_t i=0;i<sizeof(RemotePacket);i++) {
            *pkg_ptr++ = (uint8_t)LoRa.read();
        }
        _outgoing.signal = LoRa.packetSnr();
        _lowpass_signal = LoRa.packetSnr() * SIGNAL_LOWPASS + _lowpass_signal * (1 - SIGNAL_LOWPASS);
        _incomming_valid = true;
    } else {
        _incomming_valid = false;
    }
}

void RemoteClass::_writePacket() {
    uint8_t* pkg_ptr = (uint8_t*)&_outgoing;
    LoRa.beginPacket();
    LoRa.write(pkg_ptr, sizeof(RemotePacket));
    LoRa.endPacket();
    _outgoing.msg_id++;
}

bool RemoteClass::recieve() {
    _readPacket();
    return _incomming_valid;
};

bool RemoteClass::transmit() {
    _writePacket();
    return true;
}

void RemoteClass::setThruster(Move state) {
    _outgoing.thruster = state;
}

Move RemoteClass::getThruster() {
    return _incomming_valid ? _incomming.thruster : UNKNOWN;
}

void RemoteClass::setWindlass(Move state) {
    _outgoing.windlass = state;
}
Move RemoteClass::getWindlass() {
    return _incomming_valid ? _incomming.windlass : UNKNOWN;
}

void RemoteClass::setDepth(float depth) {
    _outgoing.depth = depth ;
}

float RemoteClass::getDepth() {
    return _incomming_valid ? _incomming.depth : NAN;
}

float RemoteClass::getSignal() {
    return _incomming_valid ? LoRa.packetRssi() : NAN;
}

float RemoteClass::getSNR() {
    float ratio = (_lowpass_signal - _sig_min) / (_sig_max - _sig_min);
    return constrain(ratio,0.0,1.0);
}

RemoteClass remote;