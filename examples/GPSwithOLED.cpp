/**
 * GPS/GNSS Data Display with OLED
 * 
 * This example demonstrates how to interface a u-blox GNSS/GPS module with the
 * Heltec ESP32 LoRaWAN V4 board and display real-time position and time data
 * on a 128x64 OLED screen.
 * 
 * Features:
 * - GNSS module initialization and power management
 * - UART serial communication at 9600 baud
 * - Real-time position tracking (latitude, longitude)
 * - Time synchronization from GPS signal
 * - OLED display with search progress indicator
 * - Antenna status monitoring (open/shorted detection)
 * - Uptime counter display
 * - Serial output for debugging
 * 
 * Hardware Pins (Heltec V4):
 * - GNSS RX: GPIO 39 (receives data from GPS module)
 * - GNSS TX: GPIO 38 (sends commands to GPS module)
 * - GNSS RST: GPIO 42 (GPS reset pin)
 * - GNSS WAKE: GPIO 40 (GPS wake pin)
 * - VGNSS_CTRL: GPIO 34 (GPS power control, active LOW)
 * - OLED: I2C with predefined SDA_OLED, SCL_OLED, RST_OLED pins
 * 
 * Required Libraries:
 * - Heltec ESP32 Dev-Boards (for OLED and board support)
 * - TinyGPS++ (for GPS data parsing)
 * 
 * Display Output:
 * - Searching state: "Searching GPS ..." with progress bar
 * - Locked state: Time (HH:MM:SS.CS), Latitude, Longitude
 * - Top right: Antenna status (ANT OK / ANT OPEN)
 * - Bottom right: Device uptime in seconds
 * 
 * Author: Paul Marx
 * Date: February 2026
 * License: MIT
 */

#include <Arduino.h>
#include "HT_TinyGPS++.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

TinyGPSPlus GPS;

// UART Pins (final bestätigt)
#define GNSS_RX 39   // ESP32 RX <- GNSS_TX
#define GNSS_TX 38   // ESP32 TX -> GNSS_RX

// Power/Control (final bestätigt)
#define VGNSS_CTRL 34   // active LOW
#define GNSS_WAKE  40   // active HIGH
#define GNSS_RST   42
#define GNSS_PPS   41   // optional

bool antennaOpen = false;
uint32_t lastAntennaMsg = 0;

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

void VextON()  { pinMode(Vext, OUTPUT); digitalWrite(Vext, LOW); }
void VextOFF() { pinMode(Vext, OUTPUT); digitalWrite(Vext, HIGH); }

static void gnssPowerOn() {
  pinMode(VGNSS_CTRL, OUTPUT);
  digitalWrite(VGNSS_CTRL, LOW);     // ON

  pinMode(GNSS_WAKE, OUTPUT);
  digitalWrite(GNSS_WAKE, HIGH);     // WAKE

  pinMode(GNSS_RST, OUTPUT);
  digitalWrite(GNSS_RST, HIGH);
  delay(200);

  // Reset pulse
  digitalWrite(GNSS_RST, LOW);
  delay(50);
  digitalWrite(GNSS_RST, HIGH);
  delay(500);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Program started. Setting up...");

  // OLED
  VextON();
  delay(100);
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.clear();
  display.drawString(0, 0, "Initializing ...");
  display.display();

  // GNSS
  gnssPowerOn();
  Serial1.begin(9600, SERIAL_8N1, GNSS_RX, GNSS_TX);
  Serial.println("GNSS UART started @9600");
}

void loop() {
  // --- GNSS input ---
  while (Serial1.available()) {
    char c = Serial1.read();
    GPS.encode(c);

    // very lightweight GPTXT sniffing (ANTENNA status)
    static char txtBuf[80];
    static uint8_t idx = 0;

    if (c == '$') {
      idx = 0;
    }
    if (idx < sizeof(txtBuf) - 1) {
      txtBuf[idx++] = c;
      txtBuf[idx] = 0;
    }

    if (c == '\n') {
      if (strstr(txtBuf, "ANTENNA")) {
        antennaOpen = strstr(txtBuf, "OPEN");
        lastAntennaMsg = millis();
      }
      idx = 0;
    }
  }

  // --- UI update logic ---
  static uint32_t lastUi = 0;
  bool gpsUpdated =
      GPS.location.isUpdated() ||
      GPS.time.isUpdated();

  // refresh if new GPS data OR once per second
  if (gpsUpdated || millis() - lastUi >= 1000) {
    lastUi = millis();

    display.clear();

    bool hasTime = GPS.time.isValid();
    bool hasLoc  = GPS.location.isValid();

    if (!hasTime && !hasLoc) {
      static int counter = 0;
      int progress = (counter / 5) % 100;
      counter++;

      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 10, "Searching GPS ...");
      display.drawProgressBar(0, 32, 120, 10, progress);
    } else {
      char t[20], la[24], lo[24];

      if (hasTime) {
        snprintf(t, sizeof(t), "%02d:%02d:%02d.%02d",
                 (int)GPS.time.hour(),
                 (int)GPS.time.minute(),
                 (int)GPS.time.second(),
                 (int)GPS.time.centisecond());
      } else {
        snprintf(t, sizeof(t), "--:--:--.--");
      }

      if (hasLoc) {
        snprintf(la, sizeof(la), "LAT: %.6f", (double)GPS.location.lat());
        snprintf(lo, sizeof(lo), "LON: %.6f", (double)GPS.location.lng());
      } else {
        snprintf(la, sizeof(la), "LAT: ----");
        snprintf(lo, sizeof(lo), "LON: ----");
      }

      // Serial
      Serial.println(t);
      Serial.println(la);
      Serial.println(lo);

      // OLED
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, t);
      display.drawString(0, 12, la);
      display.drawString(0, 24, lo);
    }

    // --- Antenna status (top right) ---
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    if (millis() - lastAntennaMsg < 5000) {
      display.drawString(127, 0,
        antennaOpen ? "ANT OPEN" : "ANT OK");
    }

    // --- Uptime footer ---
    char up[12];
    snprintf(up, sizeof(up), "%lus", (unsigned long)(millis() / 1000));
    display.drawString(127, 54, up);

    display.display();
  }
}
