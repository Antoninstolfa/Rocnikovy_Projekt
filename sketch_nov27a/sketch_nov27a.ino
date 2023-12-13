#include <SPI.h>
#include <Adafruit_GFX.h>
// LED matice s MAX7219

// připojení potřebné knihovny
#include "LedControl.h"
// nastavení propojovacích pinů
#define DIN 10
#define CS  11
#define CLK 12

#define pinVcc1 4
#define pinOut1 3
#define pinGnd1 2

#define pinVcc2 13
#define pinOut2 9
#define pinGnd2 8

// proměnná pro uložení času přerušení
unsigned long casPreruseni = 0;
// proměnná pro uložení stavu paprsku
bool stav1 = 0;
bool stav2 = 0;
// tato proměnná údává počet matic,
// počítání začíná od nuly
#define mojeMat 0
// inicializace LED matice z knihovny
LedControl ledMat=LedControl(DIN, CLK, CS, mojeMat);
// proměnná typu pole, 1 označují rozsvícené LED
byte cislo0[8]={B00000000,
                B00011000,
                B00100100,
                B00100100,
                B00100100,
                B00100100,
                B00011000,
                B00000000};

byte cislo1[8]={B00000000,   
                B00001000,                      
                B00011000,
                B00101000,
                B00001000,
                B00001000,
                B00011100,                      
                B00000000};

byte cislo2[8]={B00000000,                     
                B00011000,
                B00100100,
                B00000100,
                B00001000,
                B00010000,
                B00111100,    
                B00000000};

byte cislo3[8]={B00000000,
                B00011000,
                B00100100,
                B00001000,
                B00000100,
                B00100100,
                B00011000,
                B00000000};

void setup() {
// inicializace komunikace po sériové lince
  // rychlostí 9600 baud
  Serial.begin(9600);
  // nastavení směrů propojovacích pinů
  pinMode(pinVcc1, OUTPUT);
  pinMode(pinOut1,  INPUT);
  pinMode(pinGnd1, OUTPUT);

  pinMode(pinVcc2, OUTPUT);
  pinMode(pinOut2, INPUT);
  pinMode(pinGnd2, OUTPUT);

  // nastavení napájecích pinů pro správné napájení modulu
  digitalWrite(pinGnd1, LOW);
  digitalWrite(pinVcc1, HIGH);

  digitalWrite(pinGnd2, LOW);
  digitalWrite(pinVcc2, HIGH);
  // krátká pauza pro ustálení stavu modulu
  delay(10);
  // nastavení přerušení na pin Out
  // při sestupné hraně (log1->log0) se vykoná program prerus
  attachInterrupt(digitalPinToInterrupt(pinOut1), prerus, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinOut2), prerus, FALLING);

  // probuzení matice pro zahájení komunikace
  ledMat.shutdown(mojeMat,false);
  // nastavení LED matice na střední svítivost (0-15)
  ledMat.setIntensity(mojeMat,7);
  // vypnutí všech LED diod na matici
  ledMat.clearDisplay(mojeMat);
  

}

void loop() {
// načteme aktuální stav pinu Out,
  // kdy 1 znamená detekovaný paprsek
  stav1 = digitalRead(pinOut1);
  stav2 = digitalRead(pinOut2);
  // pravidelná informace o času od poslední detekce
  Serial.print("Cas od posledni detekce přerušení: ");
  Serial.print(millis() - casPreruseni);
  Serial.println(" ms.");
 
  if(stav1 == LOW && stav2 == LOW)
   {
      for(int i=0; i<8; i++) {
          ledMat.setRow(mojeMat,i,cislo0[i]);
      }
   }
   else if(stav1 == HIGH || stav2 == HIGH)
   {
      for(int i=0; i<8; i++) {
            ledMat.setRow(mojeMat,i,cislo1[i]);
      }
   }

   /*if(stav2 == LOW)
   {
      for(int i=0; i<8; i++) {
          ledMat.setRow(mojeMat,i,cislo2[i]);
      }
   }
   else if(stav2 == HIGH)
   {
      for(int i=0; i<8; i++) {
            ledMat.setRow(mojeMat,i,cislo1[i]);
      }
   }*/
  // pauza před novým během smyčky
  delay(1000);


  /*// vykreslení znaku z proměnné
  for(int i=0; i<8; i++) {
    ledMat.setRow(mojeMat,i,pismenoA[i]);
  }
  delay(1000);
  // vypnutí všech LED na matici
  ledMat.clearDisplay(mojeMat);
  // vykreslení LED diod v daném řádku, zde čtvrtý řádek
  // (řádky jsou číslovány od 0 do 7)
  ledMat.setRow(mojeMat,3,B10101010);
  delay(1000);
  // vykreslení LED diod v daném sloupci, zde šestý sloupec
  // (sloupce jsou číslovány od 0 do 7)
  ledMat.setColumn(mojeMat,5,B10111010);
  delay(1000);
  // vykreslení jedné zvolené LED diody, zde krajní dioda [8,8]
  ledMat.setLed(mojeMat,7,7,true);
  delay(1000);
  // ukázka změny intenzity svícení v celém rozsahu 0 až 15
  for(int i=0; i<16; i++) {
    ledMat.setIntensity(mojeMat,i);
    delay(200);
  }
  // nastavení intenzity na střední svit
  ledMat.setIntensity(mojeMat,7);
  // vypnutí všech LED na matici
  ledMat.clearDisplay(mojeMat);*/
}

void prerus() {
  // pokud byl před přerušením detekovaný paprsek,
  // vytiskneme informace o jeho přerušení
  if (stav1 == 1 || stav2 == 1) {
    // výpis informace při detekci
    Serial.println("Detekovana preruseni laseru!");
    // uložení času aktivace
    casPreruseni = millis();
  }
}