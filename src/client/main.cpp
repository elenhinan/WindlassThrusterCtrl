#include "main.h"

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI display(U8G2_R0, OLED_CS, OLED_DC, OLED_RST);

void setup() {
  delay(1000);

  // setup display
  if(!display.begin()) {
    while (true);
  }

  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // init radio
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

  if (remote.recieve()) {
    //interface._dispThruster();
    interface._dispWindlass();

    //float bat_voltage = analogRead(BAT_V) * VBAT_CALIBRATION;
    // Serial.print("Vbat: ");
    // Serial.println(bat_voltage);
  }
  
}

void sleep() {
  // set radio to sleep mode
  LoRa.sleep();

  // display off
  display.setPowerSave(1);

  // enable interrupt on menu pin for waking up
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_OK), wakeup, LOW);

  // enter sleep mode of MCU
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void wakeup()
{
  // detach interrupt from button
  detachInterrupt(digitalPinToInterrupt(PIN_BTN_OK)); 
  // turn radio to idle when waking up
  LoRa.idle();
  // display on
  display.setPowerSave(0);
  // reset auto off timer
  lastInput = millis();
}