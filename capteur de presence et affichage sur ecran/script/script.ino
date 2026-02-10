#include <Wire.h>
#include "rgb_lcd.h"
#include "Ultrasonic.h"

rgb_lcd lcd;
Ultrasonic ultrasonic(7);
bool blinkState = true;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(0, 128, 255);
    lcd.print("Test Distance");
    delay(1000);
}

void loop() {
    // On récupère la valeur en pouces qui, elle, fonctionne
    long inches = ultrasonic.MeasureInInches();
    
    // On calcule manuellement les centimètres
    // 1 inch = 2.54 cm
    long cm = inches * 2.54; 

    // Debug série
    Serial.print("Inches: "); Serial.print(inches);
    Serial.print(" | CM calc: "); Serial.println(cm);

    // Affichage LCD
    lcd.clear();
    
    
    lcd.setCursor(0, 0);
    lcd.print("Cm: ");
    lcd.print(cm);

    // Alerte couleur
    if (cm > 0 && cm < 20) {
      if(blinkState){
        lcd.setRGB(255, 0, 0); // Rouge si proche
      }else{
        lcd.setRGB(0, 0, 255);
      }
      blinkState = !blinkState;
    } else {
        lcd.setRGB(0, 128, 255); // Bleu
    }

    delay(300);
}