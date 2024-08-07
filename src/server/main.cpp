#include "main.h"

Thruster thruster("Thruster", THR_PIN_STARBOARD_OUT, THR_PIN_PORT_OUT, THR_PIN_STARBOARD_IN, THR_PIN_PORT_IN);
Windlass windlass("Windlass", WL_PIN_UP_OUT, WL_PIN_DOWN_OUT, WL_PIN_UP_IN, WL_PIN_DOWN_IN,  WL_PIN_CHAIN_A, WL_PIN_CHAIN_B);

void setup() {
  delay(2000);

  #ifdef DEBUG
  Serial.begin(9600);
  #endif

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
  windlass.setMeterPerPulse(CHAIN_PITCH*GYPSY_LINKS);
  windlass.setChainLength(CHAIN_LENGTH);
  if(!windlass.begin()) {
    while (true);                       // if failed, do nothing
  }
  windlass.enable();

  // setup radio control
  radio.begin();

  // setup webserver control
  webserver.begin();

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  unsigned long now = millis();
  
  bool rx = radio.recieve();
  if (radio.isValid()) {
    analogWrite(LED_BUILTIN, led_power--);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  webserver.recieve();
  // update outputs
  if (now - last_update > HW_INTERVAL) {

    // if any radio commands recieved, pass them on
    thruster.input(webserver.getThruster());
    windlass.input(webserver.getWindlass());
    thruster.input(radio.getThruster());
    windlass.input(radio.getWindlass());

    // update thruster and windlass outputs
    thruster.update();
    windlass.update();

    radio.setThruster(thruster.getState());
    radio.setWindlass(windlass.getState());
    radio.setDepth(windlass.getChain());
    webserver.setThruster(thruster.getState());
    webserver.setWindlass(windlass.getState());
    webserver.setDepth(windlass.getChain());

    last_update = now;            // timestamp the message
  }

  // reply to rf
  if (rx) {
    radio.transmit(true);
  }
  webserver.transmit();
}