#include "main.h"

/*
  LoRa Duplex communication

  Sends a message every half second, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Uses readString() from Stream class to read payload. The Stream class'
  timeout may affect other functuons, like the radio's callback. For an

  created 28 April 2017
  by Tom Igoe
*/

RemoteCtrl remote;
Adafruit_SSD1306 display(OLED_WIDTH, OLED_WIDTH, &SPI, OLED_DC, OLED_CS, OLED_RST);

void setup() {
  delay(2000);
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  // init led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // init radio
  remote.begin();

  // setup buttons
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);

  // setup display
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("SSD1306 init failed");
    while (true);
  }
  display.display();
  delay(1000);

  // done
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  unsigned long now = millis();

  if (remote.recieve()) {
    display.clearDisplay();
    display.print("Thruster: ");
    display.println(remote.getThruster());
    display.print("Windlass: ");
    display.println(remote.getWindlass());
    display.print("Depth: ");
    display.println(remote.getDepth());
    display.print("Signal: ");
    display.println(remote.getSignal());

    float bat_voltage = analogRead(BAT_V) * 2 * 3.3 / 1024;

    display.print("Vbat: ");
    display.println(bat_voltage);
  }

  if (now - lastTime > INTERVAL) {
    bool up    = !digitalRead(BTN_UP);
    bool down  = !digitalRead(BTN_DOWN);
    bool left  = !digitalRead(BTN_LEFT);
    bool right = !digitalRead(BTN_RIGHT);
    bool menu  = !digitalRead(BTN_MENU);

    if (up||down||left||right||menu) {
      lastInput = now;
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }

    remote.setWindlass(hwinput(BTN_UP, BTN_DOWN));
    remote.setThruster(hwinput(BTN_LEFT, BTN_RIGHT));
    remote.setDepth(0);
    remote.transmit();

    if (now - lastInput > AUTO_OFF) {
      sleep();
    }
  }

  lastTime = now;            // timestamp the message
}

void sleep() {
  // set radio to sleep mode
  LoRa.sleep();

  // display off
  display.ssd1306_command(SSD1306_DISPLAYOFF); // To switch display off

  // enable interrupt on menu pin for waking up
  attachInterrupt(digitalPinToInterrupt(BTN_MENU), wakeup, LOW);

  // enter sleep mode of MCU
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void wakeup()
{
  // detach interrupt from button
  detachInterrupt(digitalPinToInterrupt(BTN_MENU)); 
  // turn radio to idle when waking up
  LoRa.idle();
  // display on
  display.ssd1306_command(SSD1306_DISPLAYON); // To switch display back on
  // reset auto off timer
  lastInput = millis();
}