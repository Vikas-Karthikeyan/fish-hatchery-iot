/************************************************************
  ESP32  ▶ ThingSpeak – Fish-Hatchery Demo
  Sensors:
    • DHT22  (Temp & RH)    -> GPIO 15
    • Pot-1  (Soil Moisture)-> GPIO 34 (ADC1_CH6)
    • Pot-2  (Water Purity) -> GPIO 35 (ADC1_CH7)
  ThingSpeak fields:
    field1 = Temperature  (°C)
    field2 = Humidity     (%)
    field3 = Soil Moisture (raw 0-4095)
    field4 = Water Purity  (raw 0-4095)
************************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include "DHTesp.h"

// ─── USER SETTINGS ────────────────────────────────────────
const char* ssid     = "Wokwi-GUEST";        // e.g. "Wokwi-GUEST"
const char* password = "";    // blank for open network
const char* apiKey   = "LFX6H2R3GXOK7HSO";    // ThingSpeak WRITE key
// ThingSpeak update URL (HTTP, port 80)
const char* server   = "http://api.thingspeak.com/update";

// ─── PIN DEFINITIONS ──────────────────────────────────────
constexpr uint8_t DHT_PIN           = 15;
constexpr uint8_t SOIL_PIN          = 34;   // ADC1
constexpr uint8_t WATER_PURITY_PIN  = 35;   // ADC1

// ─── OBJECTS ──────────────────────────────────────────────
DHTesp dht;

// ─── SETUP ────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);

  // Initialise DHT22
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // Wi-Fi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  // Configure ADC pins (optional but explicit)
  pinMode(SOIL_PIN,  INPUT);
  pinMode(WATER_PURITY_PIN, INPUT);
}

// ─── MAIN LOOP ────────────────────────────────────────────
void loop() {
  // ── Read sensors ───────────────────────────────────────
  TempAndHumidity dhtData = dht.getTempAndHumidity();
  int soilRaw   = analogRead(SOIL_PIN);          // 0–4095
  int purityRaw = analogRead(WATER_PURITY_PIN);  // 0–4095

  // Log to Serial Monitor
  Serial.printf("Temp: %.1f °C | Hum: %.1f %% | Soil: %d | Purity: %d\n",
                dhtData.temperature, dhtData.humidity, soilRaw, purityRaw);

  // ── Send to ThingSpeak ─────────────────────────────────
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) +
                 "?api_key=" + apiKey +
                 "&field1=" + String(dhtData.temperature, 1) +
                 "&field2=" + String(dhtData.humidity, 1) +
                 "&field3=" + String(soilRaw) +
                 "&field4=" + String(purityRaw);

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("ThingSpeak response: " + String(httpCode));
    } else {
      Serial.println("HTTP error: " + http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected – skipping upload.");
  }

  // ThingSpeak free tier: 15 s minimum interval
  delay(2000);
}
