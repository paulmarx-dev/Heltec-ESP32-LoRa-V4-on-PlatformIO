/**
 * GNSS Module Communication Debugger
 * 
 * This diagnostic tool helps troubleshoot GNSS/GPS module communication issues
 * on the Heltec ESP32 LoRaWAN V4 board. It systematically tests all combinations
 * of power states, wake signal levels, pin configurations, and baud rates to
 * identify correct settings.
 * 
 * What It Does:
 * - Tests 2 power states (HIGH/LOW on VGNSS_CTRL pin)
 * - Tests 2 wake signal levels (HIGH/LOW on GNSS_WAKE pin)
 * - Tests both possible pin configurations (Pin Variant A & B)
 * - Tests 3 common baud rates (9600, 38400, 115200)
 * - Captures and displays raw UART data in HEX + ASCII format
 * - Counts received bytes for each test combination
 * 
 * Usage:
 * 1. Upload this sketch to your Heltec V4 board
 * 2. Open the Serial Monitor (9600 baud initially)
 * 3. The sketch will cycle through all test combinations automatically
 * 4. Watch for valid GNSS sentences (e.g., $GPRMC, $GPGGA) in the output
 * 5. Note the settings that produce valid data:
 *    - POWER state (HIGH/LOW)
 *    - WAKE state (HIGH/LOW)
 *    - Pin variant (A or B)
 *    - Baud rate
 * 6. Use these settings in your actual GNSS code
 * 
 * Important Notes:
 * - This sketch runs in setup() only and outputs to Serial (115200 baud)
 * - UART data is captured on Serial1 (GNSS module)
 * - Each test lasts 3 seconds of data capture
 * - Valid GNSS data typically starts with $ (0x24) followed by letters
 * - Common patterns: $GPRMC, $GPGGA, $GPGSA, $GPGSV
 * 
 * Hardware Pins (Heltec V4):
 * - Serial RX/TX: Varies by test (see RXA/TXA and RXB/TXB)
 * - GNSS_CTRL: GPIO 34 (power control, active LOW in normal use)
 * - GNSS_WAKE: GPIO 40 (wake signal, active HIGH in normal use)
 * - GNSS_RST: GPIO 42 (hardware reset pin)
 * 
 * Output Format:
 * Each line shows: [HEX bytes] [ASCII representation]
 * Example: "47 50 52 4D 43 . GPRMC"
 * 
 * Author: Paul Marx
 * Date: February 2026
 * License: MIT
 */

#include <Arduino.h>

#define VGNSS_CTRL 34
#define GNSS_WAKE  40
#define GNSS_RST   42

// UART-Pins (Variante A: rx=39 tx=38)
static const int RXA = 39;
static const int TXA = 38;

// UART-Pins (Variante B: rx=38 tx=39) – zum Gegentest
static const int RXB = 38;
static const int TXB = 39;

void pulseReset() {
  pinMode(GNSS_RST, OUTPUT);
  digitalWrite(GNSS_RST, LOW);
  delay(80);
  digitalWrite(GNSS_RST, HIGH);
  delay(500);
}

void setWake(bool level) {
  pinMode(GNSS_WAKE, OUTPUT);
  digitalWrite(GNSS_WAKE, level ? HIGH : LOW);
  delay(200);
}

void setGnssPower(bool level) {
  pinMode(VGNSS_CTRL, OUTPUT);
  digitalWrite(VGNSS_CTRL, level ? HIGH : LOW);
  delay(300);
}

void dumpForMs(uint32_t ms) {
  uint32_t endAt = millis() + ms;
  uint32_t cnt = 0;
  while (millis() < endAt) {
    while (Serial1.available()) {
      uint8_t b = (uint8_t)Serial1.read();
      cnt++;

      // HEX
      if (b < 16) Serial.print('0');
      Serial.print(b, HEX);
      Serial.print(' ');

      // ASCII daneben (druckbare Zeichen)
      if (b >= 32 && b <= 126) Serial.write(b);
      else Serial.print('.');
      Serial.print(' ');
    }
  }
  Serial.println();
  Serial.print("bytes read: ");
  Serial.println(cnt);
}

void tryCombo(const char* name, int rx, int tx, int baud) {
  Serial.println("--------------------------------");
  Serial.print(name);
  Serial.print("  rx=");
  Serial.print(rx);
  Serial.print(" tx=");
  Serial.print(tx);
  Serial.print(" baud=");
  Serial.println(baud);

  Serial1.end();
  delay(50);
  Serial1.begin(baud, SERIAL_8N1, rx, tx);
  delay(200);

  dumpForMs(3000);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println("GNSS bring-up debug");

  // Test alle Kombinationen kurz:
  for (int p = 0; p < 2; p++) {            // power HIGH/LOW
    for (int w = 0; w < 2; w++) {          // wake HIGH/LOW
      Serial.println("================================================================");
      Serial.println("================================================================");
      Serial.println("================================================================");
      Serial.print("POWER=");
      Serial.print(p ? "HIGH" : "LOW");
      Serial.print("  WAKE=");
      Serial.println(w ? "HIGH" : "LOW");

      setGnssPower(p);
      setWake(w);
      pulseReset();

      // Pins A
      tryCombo("PinsA", RXA, TXA, 9600);
      tryCombo("PinsA", RXA, TXA, 38400);
      tryCombo("PinsA", RXA, TXA, 115200);

      // Pins B (Gegentest)
      tryCombo("PinsB", RXB, TXB, 9600);
      tryCombo("PinsB", RXB, TXB, 38400);
      tryCombo("PinsB", RXB, TXB, 115200);
    }
  }

  Serial.println("DONE");
}

void loop() {}
