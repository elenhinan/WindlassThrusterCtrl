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

  // setup radio control
  radio.begin();

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

bool led = true;

void loop() {
  unsigned long now = millis();
  
  radio.recieve();

  // update outputs
  if (now - last_update > HW_INTERVAL) {

    // if any radio commands recieved, pass them on
    //if (wifi.recieve()) {
    //  thruster.input(wifi.getThruster());
    //  windlass.input(wifi.getWindlass());
    //}
    thruster.input(radio.getThruster());
    windlass.input(radio.getWindlass());

    // update thruster and windlass outputs
    thruster.update();
    windlass.update();

    radio.setThruster(thruster.getState());
    radio.setWindlass(windlass.getState());
    radio.setDepth(windlass.getDepth());


    last_update = now;            // timestamp the message
  }

  // transmit if needed
  bool tx = radio.transmit();
  digitalWrite(LED_BUILTIN, tx);
}