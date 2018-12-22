


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

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "pass.h"
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT11
#define DHTPIN 2
#define RELAYPIN 5

String eventType, eventPath, eventData;
DHT dht(DHTPIN, DHTTYPE);
float lastTemperature = 0;
int relayState = 0;

void setup() {
    pinMode(RELAYPIN, OUTPUT);
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.stream("/test");
  dht.begin();
}

int n = 0;
int currentMinutes=1;

void loop() {
  relayState++;
  if (relayState ==2){
    relayState=0;
  }
  digitalWrite(RELAYPIN, relayState);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("ºC Humidity: ");
    Serial.println(h);
    if (lastTemperature != t || true) {
      lastTemperature = t;
      Serial.print("Sending temperature: ");
      Serial.println(t);
      Firebase.pushFloat("temperature", t);
      delay(1000);
      Serial.print("Sending minutes: ");
      Serial.println(currentMinutes);
      Firebase.pushInt("timestamp", currentMinutes);
      
      // handle error
      if (Firebase.failed()) {
        Serial.print("setting /temperature failed:");
        Serial.println(Firebase.error());
        return;
      }
    }
  }

//
//  if (Firebase.available()) {
//    FirebaseObject event = Firebase.readEvent();
//
//    Serial.print("event: ");
//    eventType = event.getString("type");
//    Serial.println(eventType);
//    eventPath = event.getString("path");
//    Serial.print("path: ");
//    Serial.println(eventPath);
//    eventData = event.getString("data");
//    Serial.print("data: ");
//    Serial.println(eventData);
//  }
  if (Firebase.failed()) {
    Serial.print("failed:");
    Serial.println(Firebase.error());

  }
  delay(2000);
currentMinutes+=1;

  //  // set value
  //  Firebase.setFloat("number", 42.0);
  //  // handle error
  //  if (Firebase.failed()) {
  //      Serial.print("setting /number failed:");
  //      Serial.println(Firebase.error());
  //      return;
  //  }
  //  delay(1000);
  //
  //  // update value
  //  Firebase.setFloat("number", 43.0);
  //  // handle error
  //  if (Firebase.failed()) {
  //      Serial.print("setting /number failed:");
  //      Serial.println(Firebase.error());
  //      return;
  //  }
  //  delay(1000);
  //
  //  // get value
  //  Serial.print("number: ");
  //  Serial.println(Firebase.getFloat("number"));
  //  delay(1000);
  //
  //  // remove value
  //  Firebase.remove("number");
  //  delay(1000);
  //
  //  // set string value
  //  Firebase.setString("message", "hello world");
  //  // handle error
  //  if (Firebase.failed()) {
  //      Serial.print("setting /message failed:");
  //      Serial.println(Firebase.error());
  //      return;
  //  }
  //  delay(1000);
  //
  //  // set bool value
  //  Firebase.setBool("truth", false);
  //  // handle error
  //  if (Firebase.failed()) {
  //      Serial.print("setting /truth failed:");
  //      Serial.println(Firebase.error());
  //      return;
  //  }
  //  delay(1000);
  //
  //  // append a new value to /logs
  //  String name = Firebase.pushInt("logs", n++);
  //  // handle error
  //  if (Firebase.failed()) {
  //      Serial.print("pushing /logs failed:");
  //      Serial.println(Firebase.error());
  //      return;
  //  }
  //  Serial.print("pushed: /logs/");
  //  Serial.println(name);
  
}
