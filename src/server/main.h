#include <Arduino.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "pins.h"
#include "config.h"
#include "common.h"
#include "thruster.h"
#include "windlass.h"
#include "remote.h"

unsigned long lastTime = 0;            // last send time
char sendBuffer[64];

void setup();
void loop();
void sendMessage(String outgoing);
void onReceive(int packetSize);