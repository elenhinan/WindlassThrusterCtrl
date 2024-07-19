#include <Arduino.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "pins.h"
#include "config.h"
#include "common.h"
#include "thruster.h"
#include "windlass.h"
#include "lora32u4/radio.h"
#include "webserver.h"

unsigned long last_update = 0;            // last update time

char sendBuffer[64];
int led_power=0;

void setup();
void loop();
void sendMessage(String outgoing);
void onReceive(int packetSize);