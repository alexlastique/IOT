#include <SPI.h> 
#include <Ethernet.h> 
#include <PubSubClient.h>
#include <Wire.h> 
#include "rgb_lcd.h" 
#include "Ultrasonic.h" 
#include "DHT.h"


// --- CONFIGURATION RÉSEAU --- // Remplace les XX par l'adresse IP de ton serveur Mosquitto 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress mqtt_server(10, 160, 24, 236);

// Configuration des branchements 
#define DHTPIN 2
#define DHTTYPE DHT11 
#define SOUND_PIN A0 
#define LIGHT_PIN A2

rgb_lcd lcd;
Ultrasonic ultrasonic(7);
DHT dht(DHTPIN, DHTTYPE);

EthernetClient ethClient; PubSubClient client(ethClient);

void reconnect() { 
  while (!client.connected()) { 
    lcd.setRGB(255, 100, 0); // Orange 
    if (client.connect("Arduino_Station")) { 
      lcd.setRGB(0, 255, 0); // Vert 
    } else { 
      delay(5000); 
    } 
  } 
}


bool blinkState = true;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
  lcd.setRGB(0, 255, 0);
  lcd.print("Systeme v4 OK");
  
  if (Ethernet.begin(mac) == 0) { 
    Ethernet.begin(mac, IPAddress(10, 160, 24, 236)); 
  }

  client.setServer(mqtt_server, 1883); 
  delay(1500);
}

void loop() {  // Lecture des 4 capteurs 
  if (!client.connected()) { reconnect(); } client.loop();

  // 1. Lecture 
  float cm = ultrasonic.MeasureInInches() * 2.54;
  float h = dht.readHumidity(); 
  float t = dht.readTemperature(); 
  int sound = analogRead(SOUND_PIN);
  int light = analogRead(LIGHT_PIN);


  // 2. Création du message texte 
  String message = "{'Dist':" + String((int)cm) + ", 'Temp':" + String((int)t) + ", 'Hum':" + String((int)h) + ", 'Son':" + String(sound) +", 'Lum':" + String(light) +  "}";  

  // 3. Envoi MQTT (Équivalent mosquitto_pub) 
  client.publish("donne/topic", message.c_str());

  // 4. Affichage LCD 
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("MQTT: Envoye"); lcd.setCursor(0, 1); lcd.print("T:"); lcd.print((int)t); lcd.print("C H:"); lcd.print((int)h); lcd.print("%");


  // Affichage sur l'écran LCD 
  lcd.clear(); 
  lcd.setCursor(0, 0); 
  lcd.print("D:"); 
  lcd.print((int)cm); 
  lcd.print(" S:"); 
  lcd.print(sound); 
  lcd.print(" L:"); 
  lcd.print(light);
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print((int)t);
  lcd.print("C H:");
  lcd.print((int)h);
  lcd.print("%");

  // Gestion des alertes couleurs 
  if ((cm > 0 && cm < 20) || sound > 600) { 
    if(blinkState) 
      lcd.setRGB(255, 0, 0); // Rouge 
    else 
      lcd.setRGB(0, 0, 255); // Eteint 
      blinkState = !blinkState;
    } 
    else if (light < 100) { 
      lcd.setRGB(50, 0, 100); // Violet si noir 
    }
  else { 
    lcd.setRGB(0, 128, 255); // Bleu par défaut 
  }

  delay(100);
}