#include <WiFi.h>
#include <MQTT.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"
#include <NusabotSimpleTimer.h>

WiFiClient net;
MQTTClient client;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C 16x2
NusabotSimpleTimer timer;
DHTesp dhtSensor;

const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

// Pin GPIO
const int pinRed = 2;
const int pinGreen = 4;
const int pinBlue = 16;
const int pinDht = 33; // DHT22 sensor

void setup() {
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);

  lcd.init();
  lcd.backlight();
  dhtSensor.setup(pinDht, DHTesp::DHT22);
  WiFi.begin(ssid, pass);
  client.begin("broker.emqx.io", net);

  timer.setInterval(2000, publishDht);

  connect();
}

void loop() {
  client.loop();
  timer.run();  

  if (!client.connected()) {
    connect();
  }

  delay(10);
}

void publishDht() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  int suhu = round(data.temperature);
  int kelembaban = round (data.humidity);

  Serial.println("Suhu: " + String(suhu) + "°C");
  Serial.println("---");

  client.publish("Monitoring/suhu", String(suhu), true, 1);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu:");
  lcd.setCursor(6, 0);
  lcd.print(suhu);
  lcd.print((char)223); // Simbol derajat
  lcd.print("C"); //Melakukan Publish
}

void rgb(bool red, bool green, bool blue) {
  digitalWrite(pinRed, red);
  digitalWrite(pinGreen, green);
  digitalWrite(pinBlue, blue);
}

void connect() {
  rgb(1, 0, 0); // Merah
  Serial.print("Menghubungkan ke WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan...");
  lcd.setCursor(0, 1);
  lcd.print("WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

rgb(0, 1, 0); // Hijau
  Serial.println("\nWiFi Terhubung");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Terhubung");
  delay(1000);

  while (!client.connect("clientid-unik")) {
    delay(500);
  }

  rgb(0, 0, 1); // Biru
  Serial.println("MQTT Terhubung");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Terhubung ke");
  lcd.setCursor(0, 1);
  lcd.print("MQTT Broker");
}
