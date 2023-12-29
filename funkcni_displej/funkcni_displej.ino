#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10;

#define pinVcc1 4
#define pinOut1 3
#define pinGnd1 2

#define pinVcc2 12
#define pinOut2 9
#define pinGnd2 8

// proměnná pro uložení času přerušení
unsigned long casPreruseni = 0;
// proměnná pro uložení stavu paprsku
bool stav1 = 0;
bool stav2 = 0;

int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 4;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape = "Meric Rychlosti";

int wait = 50;
int spacer = 1;
int width = 5 + spacer;
void setup() {
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

matrix.setIntensity(15);
matrix.setRotation(3);

for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {
  matrix.fillScreen(LOW);
  int letter = i / width;
  int x = (matrix.width() - 1) - i % width;
  int y = (matrix.height() - 8) / 2; // center the text vertically
  while ( x + width - spacer >= 0 && letter >= 0 ) {
  if ( letter < tape.length() ) {
  matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
  }
  letter--;
  x -= width;
  }
  matrix.write();
  delay(wait);
  }
}

int draha = 60; //mm
int cas = 0.02;//sec
int rychlost = 20;//km/h

void loop() {
  // načteme aktuální stav pinu Out,
  // kdy 1 znamená detekovaný paprsek
  stav1 = digitalRead(pinOut1);
  stav2 = digitalRead(pinOut2);

  Serial.print("Cas od posledni detekce přerušení: ");
  Serial.print(millis() - casPreruseni);
  Serial.println(" ms.");                                     //kontrola vypsaných výsledků na displeji v serial Monitor
  delay(1000);            
  Serial.print("Rychlost: ");
  Serial.println(rychlost);

   /*if(stav1 == LOW && stav2 == LOW)
   {
      matrix.fillScreen(LOW);
      char cislo = '0';
      int x = 1;//(matrix.width() - 1) - i % width;
      int y = (matrix.height() - 8) / 2;
      matrix.drawChar(x, y, cislo, HIGH, LOW, 1);
      matrix.write();       
   }
   else if(stav1 == HIGH || stav2 == HIGH)                //testování laserových snímačů
   {
      matrix.fillScreen(LOW);
      char cislo = '1';
      int x = 1;//(matrix.width() - 1) - i % width;
      int y = (matrix.height() - 8) / 2;
      matrix.drawChar(x, y, cislo, HIGH, LOW, 1);
      matrix.write();
   }*/

  String cislo = String(rychlost);
  matrix.fillScreen(LOW);

  int x = 1; // pozice nalevo
  int y = (matrix.height() - 8) / 2; // Vertikální střed

  for (int letter = 0; letter < cislo.length(); letter++) {           //výpis rychlosti na matici
    matrix.drawChar(x, y, cislo[letter], HIGH, LOW, 1);
    x += width;
  }

  matrix.write();
  delay(wait);

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

  rychlost = cas/draha;     //výpočet rychlosti
}
