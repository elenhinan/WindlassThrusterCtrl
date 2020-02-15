#include "main.h"

/*
  LoRa Duplex communication

  Sends a message every half second, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Uses readString() from Stream class to read payload. The Stream class'
  timeout may affect other functuons, like the radio's callback. For an

  created 28 April 2017
  by Tom Igoe
*/

Thruster thruster("Thruster", THR_PIN_STARBOARD_OUT, THR_PIN_PORT_OUT, THR_PIN_STARBOARD_IN, THR_PIN_PORT_IN);
Windlass windlass("Windlass", WL_PIN_UP_OUT, WL_PIN_DOWN_OUT, WL_PIN_UP_IN, WL_PIN_DOWN_IN,  WL_PIN_CHAIN_A, WL_PIN_CHAIN_B);
RemoteCtrl remote;

void setup() {
  delay(2000);
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // init Thruster(s)
  thruster.debug(true);
  thruster.setCooldown(THR_COOLDOWN);
  thruster.setTimeout(THR_TIMEOUT);
  if(!thruster.begin()) {
    Serial.println("BowThruster not enabled");
    while (true);                       // if failed, do nothing
  }
  thruster.enable();
  
  // init Thruster(s) and Windlass(es)
  windlass.debug(true);
  windlass.setCooldown(WL_COOLDOWN);
  windlass.setTimeout(WL_TIMEOUT);
  windlass.setLinkLength(0.1);
  if(!windlass.begin()) {
    Serial.println("windlass not enabled");
    while (true);                       // if failed, do nothing
  }
  windlass.enable();

  // init radio
  remote.begin();

  // setup remote control

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

bool led = true;

void loop() {
  unsigned long now = millis();

  if (now - lastTime > INTERVAL) {
    digitalWrite(LED_BUILTIN, led);
    led = !led;

    remote.recieve();
    thruster.input(remote.getThruster());
    windlass.input(remote.getWindlass());

    remote.setThruster(thruster.getState());
    remote.setWindlass(windlass.getState());
    remote.setDepth(windlass.getDepth());
    remote.transmit();

    lastTime = now;            // timestamp the message
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());

  // check manual inputs
  thruster.update();
  windlass.update();
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

   // if message is for this device, or broadcast, print details:
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}