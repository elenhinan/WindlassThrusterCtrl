#include "main.h"

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI display(U8G2_R0, PIN_OLED_CS, PIN_OLED_DC, PIN_OLED_RST);

void setup() {
  delay(2000);

  //Serial.begin(9600);

  // setup display
  if(!display.begin()) {
    while (true);
  }

  // setup battery
  battery.setPins(PIN_BAT_V, PIN_BAT_CHG);
  battery.setVoltages(BATTERY_VMIN, BATTERY_VMAX);
  battery.setCalibration(BATTERY_CALIB);
  battery.begin();

  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // init radio
  radio.setSignalRange(SIGNAL_MIN, SIGNAL_MAX);
  radio.begin();

  interface.setPins(PIN_BTN_LEFT, PIN_BTN_RIGHT, PIN_BTN_UP, PIN_BTN_DOWN, PIN_BTN_OK);
  interface.setDisplay(&display);
  interface.begin();
  interface.setState(STATUS);

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  // make button commands interrupt driven on timer
  unsigned long now = millis();
  battery.update();
  interface.update();
  bool rx = radio.recieve();
  radio.transmit();

  if (interface.getIdle() > AUTO_OFF) {
    sleep();
  }

  if (rx || now - last_refresh > MIN_REFRESH_INTERVAL) {
    interface.display();
    last_refresh = now;
  }
}

void sleep() {
  // start sleep sequence
  LoRa.sleep(); // set radio to sleep mode
  display.setPowerSave(1); // display off
  attachInterrupt(digitalPinToInterrupt(PIN_ON_BUTTON), wake, LOW); // enable interrupt on button
  SPI.end(); pinMode(MOSI, INPUT); pinMode(SCK, INPUT); // set SPI pins to high Z

  // enter sleep mode of MCU
  uint16_t cycles;
  const uint16_t cycles_min = BUTTON_WAKE / 100;
  do {
    // sleep, wake on external interrupt only
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    // on waking, count time button is pressed
    digitalWrite(LED_BUILTIN, HIGH); // positive indicator of life
    cycles = 0;
    while(!digitalRead(PIN_ON_BUTTON)) {
      cycles++;
      delay(100);
    }
    digitalWrite(LED_BUILTIN, LOW);
  } while (cycles < cycles_min); // go back to sleep if button not long-pressed

  // start wake-up sequence
  detachInterrupt(digitalPinToInterrupt(PIN_ON_BUTTON)); // detach interrupt from button
  pinMode(MOSI, OUTPUT); pinMode(SCK, OUTPUT); SPI.begin(); // restore SPI
  LoRa.idle(); // turn radio to idle when waking up
  display.setPowerSave(0); // display off
  interface.resetIdle();
}

void wake() {} // only used for external interrupt sleep-waking