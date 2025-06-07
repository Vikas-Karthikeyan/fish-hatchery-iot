#include "DHTesp.h"

const int DHT_PIN = 15;
const int soilMoisturePin = 34; // Simulated Soil Moisture Sensor
const int waterPurityPin = 35;  // Simulated Water Purity Sensor

DHTesp dht;

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22); // Use DHT22 here
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();
  Serial.print("Temperature: ");
  Serial.print(data.temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(data.humidity);
  Serial.println(" %");

  int soilValue = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture (raw): ");
  Serial.println(soilValue);

  int purityValue = analogRead(waterPurityPin);
  Serial.print("Water Purity (simulated raw): ");
  Serial.println(purityValue);

  Serial.println("-----------------------------");
  delay(1000);
}
