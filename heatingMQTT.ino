#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT11
#define DHTPIN D3
#define LED 2
#define RELAYPIN 5

const char* ssid = "LowiFD58"; // Enter your WiFi name
const char* password =  "B5NZBHWL6NY8GN"; // Enter WiFi password
const char* mqttServer = "192.168.0.23";
const int mqttPort = 1883;
const char* mqttUser = "test";
const char* mqttPassword = "vfdbdf435";

String eventType, eventPath;
int eventData;
DHT dht(DHTPIN, DHTTYPE);
float lastTemperature = 0;
int relayState = 0;
int sleepTime = 5;
char temperature[4];
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
   pinMode(RELAYPIN, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    blink(5);
    delay(500);
    
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
    dht.begin();
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.subscribe("status");
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  int status = payload[0] - '0';
      Serial.println(status);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");

  if (status != relayState){
    relayState = status;
  digitalWrite(RELAYPIN, relayState);
  digitalWrite(LED, !relayState);
  
  }
 
}
 
void loop() {
 //   blink(1);
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
      dtostrf(t, 2, 0, temperature);
       client.publish("temperature", temperature);
      delay(500);

    }
  }
  delay(500);
  client.loop();
}

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
