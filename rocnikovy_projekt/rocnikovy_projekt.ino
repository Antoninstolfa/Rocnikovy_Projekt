#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10;

#define pinVcc1 4
#define pinOut1 3
#define pinGnd1 7

#define pinVcc2 12
#define pinOut2 2
#define pinGnd2 8

// proměnná pro uložení stavu paprsku
volatile bool stav1 = false;
volatile bool stav2 = false;

int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 4;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape = "Meric Rychlosti";

int wait = 50;
int spacer = 1;
int width = 5 + spacer;

int draha = 60; // mm
unsigned long cas = 0; // ms
unsigned long casZacatek = 0;
unsigned long casKonec = 0;
float rychlost = 20.0; // km/h

bool preruseni1 = false;

void setup() {
  Serial.begin(9600);
  
  // nastavení směrů propojovacích pinů
  pinMode(pinVcc1, OUTPUT);
  pinMode(pinOut1, INPUT);
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
  attachInterrupt(digitalPinToInterrupt(pinOut1), preruseniLaseru1, RISING);
  attachInterrupt(digitalPinToInterrupt(pinOut2), preruseniLaseru2, RISING); // někde bude chyba (podpora pinů přerušení, pouze pin 2 a 3)

  matrix.setIntensity(15);
  matrix.setRotation(3);

  /*for (int i = 0; i < width * tape.length() + matrix.width() - 1 - spacer; i++) {
    matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically
    while (x + width - spacer >= 0 && letter >= 0) {
      if (letter < tape.length()) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write();
    delay(wait);
  }*/
}

void loop() {
  // načteme aktuální stav pinu Out,
  // kdy 1 znamená detekovaný paprsek
  stav1 = digitalRead(pinOut1);
  stav2 = digitalRead(pinOut2);

  if(casZacatek > 0 && casKonec > 0)
  {
    cas = casKonec - casZacatek;
    Serial.print("čas: ");
    Serial.println(cas);  
    delay(100);
  }

 if(stav1 == LOW && stav2 == LOW)
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
   }

  /*String cislo = String(rychlost, 1); // Zaokrouhlení rychlosti na jedno desetinné místo
  matrix.fillScreen(LOW);

  int x = 1; // pozice nalevo
  int y = (matrix.height() - 8) / 2; // Vertikální střed

  for (int letter = 0; letter < cislo.length(); letter++) {
    matrix.drawChar(x, y, cislo[letter], HIGH, LOW, 1);
    x += width;
  }

  matrix.write();
  delay(wait);*/
  preruseni1 = false;
  cas = 0;
}

void preruseniLaseru1() {
  Serial.println("Preruseni laseru 1");
  if(preruseni1 == false)
  {
    casZacatek = millis();
    preruseni1 = true;
  }
}

void preruseniLaseru2() {                                     //zkontrolovat měření času
  Serial.println("Preruseni laseru 2");
  if(preruseni1 == true)
  {
    casKonec = millis();
    preruseni1 = false;
  }
}

/*void preruseni(){
  if(stav1 == false && preruseni1 == false)
  {
    Serial.println("Preruseni laseru 1");
    casZacatek = millis();
    preruseni1 = true;
  }
  if(stav2 == false)
  {
    Serial.println("Preruseni laseru 2");
    casKonec = millis();
    preruseni1 = false;
  }
  if(casZacatek > 0 && casKonec > 0)
  {
    cas = casKonec - casZacatek;
    Serial.print("čas: ");
    Serial.println(cas);
  }
}*/