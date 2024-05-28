#include "Radio.h"

RadioClass::RadioClass() :
    _changed(true),
    _incomming_updated(false),
    _incomming_timestamp(0),
    _outgoing_timestamp(0),
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
    LoRa.setTxPower(12, PA_OUTPUT_PA_BOOST_PIN);
    // bandwidth: https://unsigned.io/understanding-lora-parameters/
    LoRa.setSignalBandwidth(500E3);
    LoRa.setSpreadingFactor(7);
    LoRa.setCodingRate4(7);
    LoRa.enableCrc();
    #ifdef DEBUG
    LoRa.onTxDone(_onTxDone);
    #endif
    //Serial.println("LoRa init succeeded.");
}

void RadioClass::_readPacket() {
    if (LoRa.parsePacket() == sizeof(RadioPacket)) {
        LoRa.readBytes((uint8_t*)&_incomming, sizeof(RadioPacket));
        _lowpass_signal = min(LoRa.packetSnr(), _incomming.signal) * SIGNAL_LOWPASS + _lowpass_signal * (1 - SIGNAL_LOWPASS);
        _outgoing.signal = LoRa.packetSnr();
        _incomming_timestamp = millis();
        _incomming_updated = true;
    } else {
        _incomming_updated = false;
    }
}

bool RadioClass::isValid() {
    unsigned long now = millis();
    return (now -_incomming_timestamp < RF_EXPIRES);// && _incomming_updated);
}

void RadioClass::_writePacket() {
    LoRa.beginPacket();
    LoRa.write((uint8_t*)&_outgoing, sizeof(RadioPacket));
    LoRa.endPacket(true); // true = non blocking mode
    _outgoing.msg_id++;
}

#ifdef DEBUG
void _onTxDone() {
    Serial.print(millis());
    Serial.println(": TX done");
}
#endif

bool RadioClass::recieve(unsigned long timeout) {
    unsigned long timeout_start = millis();
    do {
        _readPacket();
    } while (_incomming_updated == false && millis() - timeout_start <= timeout);

    return _incomming_updated;
};

bool RadioClass::transmit(bool force) {
    unsigned long now = millis();
    if (now - _outgoing_timestamp >= RF_INTERVAL || force || _changed) {
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
    return isValid() ? _lowpass_signal : NAN;
}

float RadioClass::getSNR() {
    if (isValid()) {
        float ratio = (_lowpass_signal - SIGNAL_MIN) / (SIGNAL_MAX - SIGNAL_MIN);
        return constrain(ratio,0.0,1.0);
    } else {
        return NAN;
    }
}

RadioClass radio;