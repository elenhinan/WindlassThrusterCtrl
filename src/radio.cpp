#include "Radio.h"

RadioClass::RadioClass() :
    _changed(true),
    _incomming_valid(false),
    _incomming_timestamp(0),
    _outgoing_timestamp(0),
    _sig_min(0),
    _sig_max(0),
    _lowpass_signal(0)
{
    _outgoing.sender = 'T';
    _outgoing.msg_id = 0;
}

void RadioClass::begin() {
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

void RadioClass::_readPacket() {
    uint8_t* pkg_ptr = (uint8_t*)&_incomming;

    int packetsize = LoRa.parsePacket();

    if (packetsize == sizeof(RadioPacket)) {
        for (uint8_t i=0;i<sizeof(RadioPacket);i++) {
            *pkg_ptr++ = (uint8_t)LoRa.read();
        }
        _lowpass_signal = LoRa.packetSnr() * SIGNAL_LOWPASS + _lowpass_signal * (1 - SIGNAL_LOWPASS);
        _outgoing.signal = LoRa.packetSnr();
        _incomming_timestamp = millis();
        _incomming_valid = true;
    }
    else if (packetsize != 0) {
        _incomming_valid = false;
    }
}

bool RadioClass::isValid() {
    unsigned long now = millis();
    return (now -_incomming_timestamp < RF_TIMEOUT);// && _incomming_valid);
}

void RadioClass::_writePacket() {
    uint8_t* pkg_ptr = (uint8_t*)&_outgoing;
    LoRa.beginPacket();
    LoRa.write(pkg_ptr, sizeof(RadioPacket));
    LoRa.endPacket();
    _outgoing.msg_id++;
}

bool RadioClass::recieve() {
    _readPacket();
    return _incomming_valid;
};

bool RadioClass::transmit() {
    unsigned long now = millis();
    if (now - _outgoing_timestamp < RF_MIN_INTERVAL)
        return false;
    else if (_changed || now - _outgoing_timestamp >= RF_MAX_INTERVAL) {
        _writePacket();
        _changed = false;
        _outgoing_timestamp = now;
        return true;
    } else {
        return false;
    }
}

void RadioClass::setThruster(Move state) {
    if (_outgoing.thruster != state) {
        _changed = true;
        _outgoing.thruster = state;
    }
}

Move RadioClass::getThruster() {
    return isValid() ? _incomming.thruster : UNKNOWN;
}

void RadioClass::setWindlass(Move state) {
    if (_outgoing.windlass != state) {
        _changed = true;
        _outgoing.windlass = state;
    }
}
Move RadioClass::getWindlass() {
    return isValid() ? _incomming.windlass : UNKNOWN;
}

void RadioClass::setDepth(float depth) {
    if (_outgoing.depth != depth) {
        _changed = true;
        _outgoing.depth = depth ;
    }
}

float RadioClass::getDepth() {
    return isValid() ? _incomming.depth : NAN;
}

float RadioClass::getSignal() {
    return isValid() ? LoRa.packetRssi() : NAN;
}

float RadioClass::getSNR() {
    float ratio = (_lowpass_signal - _sig_min) / (_sig_max - _sig_min);
    return constrain(ratio,0.0,1.0);
}

RadioClass radio;