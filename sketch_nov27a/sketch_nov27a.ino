#include <SPI.h>
#include <Adafruit_GFX.h>
// LED matice s MAX7219

// připojení potřebné knihovny
#include "LedControl.h"
// nastavení propojovacích pinů
#define DIN 10
#define CS  11
#define CLK 12

#define pinVcc 2
#define pinOut 3
#define pinGnd 4
// proměnná pro uložení času přerušení
unsigned long casPreruseni = 0;
// proměnná pro uložení stavu paprsku
bool stav = 0;
// tato proměnná údává počet matic,
// počítání začíná od nuly
#define mojeMat 0
// inicializace LED matice z knihovny
LedControl ledMat=LedControl(DIN, CLK, CS, mojeMat);
// proměnná typu pole, 1 označují rozsvícené LED
byte cislo1[8]={   B00000000,   
                   B00001000,                      
                   B00011000,
                   B00101000,
                   B00001000,
                   B00001000,
                   B00111100,                      
                   B00000000};

byte cislo2[8]={  B00000000,                     
                  B00011000,
                  B00100100,
                  B00000100,
                  B00001000,
                  B00010000,
                  B00111100,    
                  B00000000};
void setup() {
// inicializace komunikace po sériové lince
  // rychlostí 9600 baud
  Serial.begin(9600);
  // nastavení směrů propojovacích pinů
  pinMode(pinVcc, OUTPUT);
  pinMode(pinOut,  INPUT);
  pinMode(pinGnd, OUTPUT);
  // nastavení napájecích pinů pro správné napájení modulu
  digitalWrite(pinGnd, LOW);
  digitalWrite(pinVcc, HIGH);
  // krátká pauza pro ustálení stavu modulu
  delay(10);
  // nastavení přerušení na pin Out
  // při sestupné hraně (log1->log0) se vykoná program prerus
  attachInterrupt(digitalPinToInterrupt(pinOut), prerus, FALLING);


  // probuzení matice pro zahájení komunikace
  ledMat.shutdown(mojeMat,false);
  // nastavení LED matice na střední svítivost (0-15)
  ledMat.setIntensity(mojeMat,7);
  // vypnutí všech LED diod na matici
  ledMat.clearDisplay(mojeMat);
  
for(int i=0; i<8; i++) {
    ledMat.setRow(mojeMat,i,cislo2[i]);
  }
}

void loop() {
// načteme aktuální stav pinu Out,
  // kdy 1 znamená detekovaný paprsek
  stav = digitalRead(pinOut);
  // pravidelná informace o času od poslední detekce
  Serial.print("Cas od posledni detekce přerušení: ");
  Serial.print(millis() - casPreruseni);
  Serial.println(" ms.");
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
  if (stav == 1) {
    // výpis informace při detekci
    Serial.println("Detekovana preruseni laseru!");
    // uložení času aktivace
    casPreruseni = millis();
  }
}