#define BLYNK_TEMPLATE_ID "TMPL6bxBLVsc-"
#define BLYNK_TEMPLATE_NAME "smart farm"
#define BLYNK_AUTH_TOKEN "8w3OQsJvX4SyBpMriYqqYkw1Vfr_y9aB"

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHT_PIN 32  // GPIO32 P32
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 34 // GPIO34 P34
#define PUMP_WATER_PIN 9 // GPIO9 SD2
#define PUMP_WATER_SWITCH V4  
#define PUMP_fertilizer_PIN 10 // GPIO10 SD3
#define PUMP_fertilizer_SWITCH V5

char auth[] = BLYNK_AUTH_TOKEN;  
char ssid[] = "esuspenhereraika?";
char pass[] = "nongchoc3rd";

BlynkTimer timer;

bool isPumpwaterOn = false;
bool isPumpfertilizerOn = false;
DHT dht(DHT_PIN, DHTTYPE);

void sendSensorData()
{
  // Soil Moisture Data
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  int soilMoisturePercentage = map(soilMoisture, 0, 4095, 100, 0);
  Serial.print("Soil Moisture Percentage: ");
  Serial.println(soilMoisturePercentage);

  Blynk.virtualWrite(V3, soilMoisturePercentage);

  // Water Pump Control
  if (isPumpwaterOn | soilMoisturePercentage < 25) {
    digitalWrite(PUMP_WATER_PIN, LOW);  // Turn pump on
  } else {
    digitalWrite(PUMP_WATER_PIN, HIGH);  // Turn pump off
  }

  // Fertilizer Pump Control
  if (isPumpfertilizerOn) {
    digitalWrite(PUMP_fertilizer_PIN, LOW);  // Turn pump on
  } else {
    digitalWrite(PUMP_fertilizer_PIN, HIGH);  // Turn pump off
  }

   // DHT11 Sensor Data
  float dhtTemp = dht.readTemperature(); 
  float dhtHum = dht.readHumidity();     

  if (isnan(dhtTemp) || isnan(dhtHum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1, dhtTemp);
  Blynk.virtualWrite(V2, dhtHum);

  Serial.print("Temp (DHT): ");
  Serial.print(dhtTemp);
  Serial.print("°C, Humidity (DHT): ");
  Serial.println(dhtHum);

}

BLYNK_WRITE(PUMP_WATER_SWITCH)
{
  isPumpwaterOn = param.asInt();
  if (isPumpwaterOn) {
    Serial.println("Pump water turned ON");
  } else {
    Serial.println("Pump water turned OFF");
  }
}

BLYNK_WRITE(PUMP_fertilizer_SWITCH)
{
  isPumpfertilizerOn = param.asInt();
  if (isPumpfertilizerOn) {
    Serial.println("Pump fertilizer turned ON");
  } else {
    Serial.println("Pump fertilizer turned OFF");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_WATER_PIN, OUTPUT);
  pinMode(PUMP_fertilizer_PIN, OUTPUT);

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, sendSensorData);

  Blynk.syncVirtual(PUMP_WATER_SWITCH);
  Blynk.syncVirtual(PUMP_fertilizer_SWITCH);
}

void loop()
{
  Blynk.run();
  timer.run();
}
