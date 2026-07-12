#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>



// Initialize the SHT31/SHT30 sensor
// Default I2C address is 0x44
// --- Configuration ---
const char* ssid = "Ranjit-AP";
const char* password = "AgS@2004!#$";

// The ID from your Google Web App URL (the part between /s/ and /exec)
const char* GScriptId = "AKfycby6Q1Z48GAdaxzs4qmC0nepOfRn-83mwZaurGeRjy9wDHqHECcdMlI9gQk2BgqeapU1"; 


const char* host = "script.google.com";
const int httpsPort = 443;


Adafruit_SHT31 sht31 = Adafruit_SHT31();
/*void setup() {
  Serial.begin(9600); // Use 115200 to avoid gibberish
  sht31.begin();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}*/


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  while (!Serial) delay(10); // Wait for serial console to open

  Serial.println("SHT30 Test Started!");

  // Start I2C on NodeMCU (D2=SDA, D1=SCL)
  Wire.begin(4, 5); 

  if (!sht31.begin(0x44)) {   // Check for sensor at address 0x44
    Serial.println("Couldn't find SHT30! Check wiring.");
    while (1) delay(1);
  }
}

void loop() {
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (!isnan(t) && !isnan(h)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(t);
    Serial.print("\t\tHum % = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read temperature or humidity");
  }
  sendData(t, h);
  //delay(1800000UL); // Wait 30 min between readings
  delay(6000);
}
void sendData(float temp, float hum) {
  WiFiClientSecure client;
  client.setInsecure(); // Required for Google Scripts

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  String url = String("/macros/s/") + GScriptId + "/exec?temp=" + String(temp) + "&hum=" + String(hum);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Data sent to Google Sheets!");
}