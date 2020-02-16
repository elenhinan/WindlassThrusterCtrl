#include <Arduino.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <LowPower.h>
//#include <Adafruit_SSD1306.h>
#include <U8g2lib.h>
#include "pins.h"
#include "config.h"
#include "common.h"
#include "tools.h"
#include "remote.h"
#include "interface.h"
#include "battery.h"

unsigned long lastTime = 0;            // last send time

void setup();
void loop();
void sleep();
void wake();