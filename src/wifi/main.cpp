#include "main.h"
#include "arduino_secrets.h"

// add flashing of server
// install https://github.com/dmjlambert/optiboot on 328
// flash via serial https://arduino.stackexchange.com/questions/61796/is-it-possible-to-upload-an-arduino-sketch-through-the-serial-port-rx-instead

AsyncWebServer server(80);

unsigned long ota_progress_millis = 0;

void setup() {
  delay(2000);

  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  // init WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  MDNS.begin(HOSTNAME);

  // init OTA
  ElegantOTA.begin(&server);    // Start ElegantOTA

  // init link
  RadioLink.begin();

  // init webserver
  server.begin();
  server.on("/status", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String buffer;
        RadioLink.getJson(buffer);
        request->send(200, "application/json", buffer);
    });
  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Handle OTA and mDNS stuff each loop
  ElegantOTA.loop();
  MDNS.update();
  RadioLink.update();
}