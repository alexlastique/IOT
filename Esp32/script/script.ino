#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "rgb_lcd.h"

// --- Configuration ---
const char* ssid = "iotg2wifi";
const char* password = "iotg2wifi";
const char* mqtt_server = "10.160.24.236";
const char* topic_subscribe = "donne/topic";

WiFiClient espClient;
PubSubClient client(espClient);
rgb_lcd lcd;

// --- Fonction de connexion au WiFi ---
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecte");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

// --- Fonction de reconnexion MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    // Tentative de connexion
    if (client.connect("ESP32_Client")) {
      Serial.println("connecte !");
      client.subscribe(topic_subscribe);
    } else {
      Serial.print("Echec, code d'erreur=");
      Serial.print(client.state());
      Serial.println(" nouvelle tentative dans 5 secondes");
      delay(5000);
    }
  }
}

// --- Reception des messages ---
void callback(char* topic, byte* payload, unsigned int length) {
  String input = "";
  for (int i = 0; i < length; i++) { input += (char)payload[i]; }
  
  // Nettoyage JSON (remplace ' par ")
  input.replace("'", "\"");

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print("Erreur JSON: ");
    Serial.println(error.f_str());
    return;
  }

  int dist = doc["Dist"];
  int temp = doc["Temp"];
  int hum  = doc["Hum"];
  int lum  = doc["Lum"];

  // Affichage Console
  Serial.printf("Distance: %d | Temp: %d | Hum: %d | Lum: %d\n", dist, temp, hum, lum);

  // Affichage LCD
  lcd.clear();
  lcd.setRGB(0, 255, 0); 
  lcd.setCursor(0, 0);
  lcd.printf("T:%dC H:%d%%", temp, hum);
  lcd.setCursor(0, 1);
  lcd.printf("Dist:%d L:%d", dist, lum);
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("Init WiFi...");
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}