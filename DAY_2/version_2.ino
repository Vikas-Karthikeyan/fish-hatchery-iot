/************************************************************
  ESP32  ▶ Fish-Hatchery Demo + OLED Display (No ThingSpeak)
  Sensors:
    • DHT22  (Temp & RH)    -> GPIO 15
    • Pot-1  (Soil Moisture)-> GPIO 34 (ADC1_CH6)
    • Pot-2  (Water Purity) -> GPIO 35 (ADC1_CH7)
************************************************************/

#include <WiFi.h>
#include "DHTesp.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─── USER SETTINGS ────────────────────────────────────────
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ─── PIN DEFINITIONS ──────────────────────────────────────
constexpr uint8_t DHT_PIN           = 15;
constexpr uint8_t SOIL_PIN          = 34;   // ADC1
constexpr uint8_t WATER_PURITY_PIN  = 35;   // ADC1

// ─── OBJECTS ──────────────────────────────────────────────
DHTesp dht;

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ─── SETUP ────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);

  // Initialise DHT22
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // OLED Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Fish Hatchery Monitor");
  display.display();
  delay(1500);

  // Wi-Fi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  // Configure ADC pins
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

  // ── Display on OLED ────────────────────────────────────
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Temp: %.1f C\n", dhtData.temperature);
  display.printf("Hum : %.1f %%\n", dhtData.humidity);
  display.printf("Soil: %d\n", soilRaw);
  display.printf("Pure: %d\n", purityRaw);
  display.display();

  delay(2000);
}
