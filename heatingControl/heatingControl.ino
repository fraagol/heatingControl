
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "pass.h"
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT11
#define DHTPIN D3
#define LED 2
#define RELAYPIN 5

String eventType, eventPath;
int eventData;
DHT dht(DHTPIN, DHTTYPE);
float lastTemperature = 0;
int relayState = 0;
int sleepTime = 5;

void setup() {
  pinMode(RELAYPIN, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    blink(5);
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.stream("/active");
  dht.begin();
}

int n = 0;
int currentMinutes = 1;

void blink(int n) {
  digitalWrite(LED, 0);
  delay(50);
  for(int i=0; i <=n; i++){
  digitalWrite(LED, 1);
  delay(50);
  digitalWrite(LED, 0);
  delay(50);
  }
}

void loop() {
  blink(1);
  digitalWrite(RELAYPIN, relayState);
  digitalWrite(LED, !relayState);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    blink(10);
  } else {
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("ºC Humidity: ");
    Serial.println(h);
    if (lastTemperature != t) {
      lastTemperature = t;
      Serial.print("Sending temperature: ");
      Serial.println(t);
      Firebase.setInt("temperature", t);
      delay(500);

      // handle error
      if (Firebase.failed()) {
        Serial.print("setting /temperature failed:");
        Serial.println(Firebase.error());
        return;
      }
    }
  }

  int active = Firebase.getInt("active"); 
  if (active != relayState){
    relayState = active;
  digitalWrite(RELAYPIN, relayState);
  digitalWrite(LED, !relayState);
  Firebase.setInt("state", relayState);
  
  }

  sleepTime = Firebase.getInt("sleepTime");

//  if (Firebase.available()) {
//    Serial.print("Stream available");
//    FirebaseObject event = Firebase.readEvent();
//    eventType = event.getString("type");
//
//    if (eventType.length() > 0) {
//      Serial.print("event: ");
//      Serial.println(eventType);
//      eventPath = event.getString("path");
//      Serial.print("path: ");
//      Serial.println(eventPath);
//      eventData = event.getInt("data");
//      Serial.print("data: ");
//      Serial.println(eventData);
//      relayState = eventData;
//      digitalWrite(RELAYPIN, relayState);
//      digitalWrite(LED, !relayState);
//      Firebase.setInt("state", relayState);
//
//    }
//  }


  if (Firebase.failed()) {
    Serial.print("Reseting after fail:");
    Serial.println(Firebase.error());
    ESP.restart();
  }
  delay(sleepTime * 1000);




}


//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseDemo_ESP8266 is a sample that demo the different functions
// of the FirebaseArduino API.
