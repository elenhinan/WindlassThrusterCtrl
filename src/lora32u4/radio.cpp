#include "Radio.h"

RadioClass::RadioClass() :
    _lowpass_signal(0)
{

}

void RadioClass::begin() {
    //Serial.println("LoRa Duplex");
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);// set CS, reset, IRQ pin
    
    if (!LoRa.begin(LORA_BAND)) {             // initialize ratio at 915 MHz
        //Serial.println("LoRa init failed. Check your connections.");
        while (true);                       // if failed, do nothing
    }
    LoRa.setSyncWord(LORA_BOAT_ID);
    LoRa.setTxPower(8, PA_OUTPUT_PA_BOOST_PIN);
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
    if (LoRa.parsePacket() == sizeof(RemotePacket)) {
        LoRa.readBytes((uint8_t*)&_incomming, sizeof(RemotePacket));
        _lowpass_signal = min(LoRa.packetSnr(), _incomming.signal) * SIGNAL_LOWPASS + _lowpass_signal * (1 - SIGNAL_LOWPASS);
        _outgoing.signal = LoRa.packetSnr();
        _incomming_timestamp = millis();
        _incomming_updated = true;
        _valid = true;
    } else {
        _incomming_updated = false;
    }
}

void RadioClass::_writePacket() {
    LoRa.beginPacket();
    LoRa.write((uint8_t*)&_outgoing, sizeof(RemotePacket));
    LoRa.endPacket(true); // true = non blocking mode
    _outgoing.msg_id++;
}

bool RadioClass::recieve(unsigned long timeout) {
    unsigned long timeout_start = millis();
    do {
        _readPacket();
    } while (_incomming_updated == false && millis() - timeout_start <= timeout);

    return _incomming_updated;
};

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