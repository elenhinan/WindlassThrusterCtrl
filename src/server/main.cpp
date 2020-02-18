#include "main.h"

Thruster thruster("Thruster", THR_PIN_STARBOARD_OUT, THR_PIN_PORT_OUT, THR_PIN_STARBOARD_IN, THR_PIN_PORT_IN);
Windlass windlass("Windlass", WL_PIN_UP_OUT, WL_PIN_DOWN_OUT, WL_PIN_UP_IN, WL_PIN_DOWN_IN,  WL_PIN_CHAIN_A, WL_PIN_CHAIN_B);

void setup() {
  delay(2000);

  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // init Thruster(s)
  //thruster.debug(true);
  thruster.setCooldown(THR_COOLDOWN);
  thruster.setTimeout(THR_TIMEOUT);
  if(!thruster.begin()) {
    while (true);                       // if failed, do nothing
  }
  thruster.enable();
  
  // init Thruster(s) and Windlass(es)
  //windlass.debug(true);
  windlass.setCooldown(WL_COOLDOWN);
  windlass.setTimeout(WL_TIMEOUT);
  windlass.setLinkLength(0.1); // 10 cm per pulse
  windlass.setChainLength(50.0); // 50 meter chain
  if(!windlass.begin()) {
    while (true);                       // if failed, do nothing
  }
  windlass.enable();

  // setup remote control
  remote.begin();

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

bool led = true;

void loop() {
  unsigned long now = millis();

  // check manual inputs
  thruster.update();
  windlass.update();

  if (now - lastTime > INTERVAL) {
    digitalWrite(LED_BUILTIN, led);
    led = !led;

    //remote.recieve();
    //thruster.input(remote.getThruster());
    //windlass.input(remote.getWindlass());

    remote.setThruster(thruster.getState());
    remote.setWindlass(windlass.getState());
    remote.setDepth(windlass.getDepth());
    remote.transmit();

    lastTime = now;            // timestamp the message
  }
}