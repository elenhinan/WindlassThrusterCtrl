#include <Arduino.h>
#include <SPI.h>              // include libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include "radiolink.h"
#include "common.h"

#define HOSTNAME "BowControl"

unsigned long last_update = 0;            // last update time

void setup();
void loop();
void sendMessage(String outgoing);
void onReceive(int packetSize);