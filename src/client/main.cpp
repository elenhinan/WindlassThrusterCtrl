#include "main.h"

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI display(U8G2_R0, PIN_OLED_CS, PIN_OLED_DC, PIN_OLED_RST);

void setup() {
  delay(1000);

  //Serial.begin(9600);

  // setup display
  if(!display.begin()) {
    while (true);
  }

  // setup battery
  battery.setPins(PIN_BAT_V);
  battery.setVoltages(BATTERY_VMIN, BATTERY_VMAX);
  battery.setCalibration(BATTERY_CALIB);

  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // init radio
  remote.setSignalRange(SIGNAL_MIN, SIGNAL_MAX);
  remote.begin();

  interface.setPins(PIN_BTN_LEFT, PIN_BTN_RIGHT, PIN_BTN_UP, PIN_BTN_DOWN, PIN_BTN_OK);
  interface.setDisplay(&display);
  interface.begin();

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  //unsigned long now = millis();
  interface.update();

  if (interface.getIdle() > AUTO_OFF) {
    sleep();
  }

  if (remote.recieve()) {
    //interface._dispThruster();
    interface._dispWindlass();
  }
}

void sleep() {
  // start sleep sequence
  LoRa.sleep(); // set radio to sleep mode
  display.setPowerSave(1); // display off
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_OK), wake, LOW); // enable interrupt on button
  
  // enter sleep mode of MCU
  uint16_t cycles;
  const uint16_t cycles_min = BUTTON_WAKE / 100;
  do {
    // sleep, wake on external interrupt only
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    // on waking, count time button is pressed
    digitalWrite(LED_BUILTIN, HIGH); // positive indicator of life
    cycles = 0;
    while(!digitalRead(PIN_BTN_OK)) {
      cycles++;
      delay(100);
    }
    digitalWrite(LED_BUILTIN, LOW);
  } while (cycles < cycles_min); // go back to sleep if button not long-pressed

  // start wake-up sequence
  detachInterrupt(digitalPinToInterrupt(PIN_BTN_OK)); // detach interrupt from button
  LoRa.idle(); // turn radio to idle when waking up
  display.setPowerSave(0); // display off
  interface.resetIdle();
}

void wake() {} // only used for external interrupt sleep-waking