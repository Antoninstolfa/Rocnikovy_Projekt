#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10;
int button1 = 5;;

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

String tape = "Meric Rychlosti pohybu";

int wait = 50;
int spacer = 1;
int width = 5 + spacer;

double draha = 1; // m
double cas = 0; 
double casZacatek = 0;
double casKonec = 0;
double rychlost; // m/s
double MAX_rychlost = 0;

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

  pinMode(button1, INPUT_PULLUP);

  // nastavení napájecích pinů pro správné napájení modulu
  digitalWrite(pinGnd1, LOW);
  digitalWrite(pinVcc1, HIGH);

  digitalWrite(pinGnd2, LOW);
  digitalWrite(pinVcc2, HIGH);
  
  // krátká pauza pro ustálení stavu modulu
  delay(10);
  
  // nastavení přerušení na pin Out
  attachInterrupt(digitalPinToInterrupt(pinOut1), preruseniLaseru1, RISING);
  attachInterrupt(digitalPinToInterrupt(pinOut2), preruseniLaseru2, RISING); 

  matrix.setIntensity(15);
  matrix.setRotation(3);

  for (int i = 0; i < width * tape.length() + matrix.width() - 1 - spacer; i++) {
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
  }
}
 bool zmereno = false;
void loop() {
  // načteme aktuální stav pinu Out,
  // kdy 1 znamená detekovaný paprsek
  stav1 = digitalRead(pinOut1);
  stav2 = digitalRead(pinOut2);
  int button1State = digitalRead(button1);

  if(button1State == LOW)
  {
    String vypisMax = String(MAX_rychlost, 1); // Zaokrouhlení rychlosti na jedno desetinné místo
    matrix.fillScreen(LOW);

    int x = 1; // pozice nalevo
    int y = (matrix.height() - 8) / 2; // Vertikální střed

    for (int letter = 0; letter < vypisMax.length(); letter++) {
      matrix.drawChar(x, y, vypisMax[letter], HIGH, LOW, 1);
      x += width;
    }

    matrix.write();
    delay(3000);
  }

  if(casZacatek > 0 && casKonec > 0 && zmereno == true)
  {
    cas = casKonec - casZacatek;
    cas = cas / 1000; // převod na sec
    Serial.print("čas: ");
    Serial.println(cas);  
    delay(100);
  }

  zmereno = false;

  String cislo = String(rychlost, 1); // Zaokrouhlení rychlosti na jedno desetinné místo
  matrix.fillScreen(LOW);

  int x = 1; // pozice nalevo
  int y = (matrix.height() - 8) / 2; // Vertikální střed

  for (int letter = 0; letter < cislo.length(); letter++) {
    matrix.drawChar(x, y, cislo[letter], HIGH, LOW, 1);
    x += width;
  }

  matrix.write();
  delay(wait);
}

void preruseniLaseru1() {
  Serial.println("Preruseni laseru 1");
  if(preruseni1 == false)
  {
    casZacatek = millis();
    Serial.print("čas začátek: ");
    Serial.println(casZacatek);
    preruseni1 = true;
  }
}

void preruseniLaseru2() {                                     
  Serial.println("Preruseni laseru 2");
  if(preruseni1 == true)
  {
    casKonec = millis();
    Serial.print("čas zkonec: ");
    Serial.println(casKonec);
    preruseni1 = false;
    zmereno = true;
    if(cas > 0)
    {
       rychlost = draha / cas;
       if(rychlost > MAX_rychlost)
       {
        MAX_rychlost = rychlost;
       }
    }
    Serial.print("výsledek: ");
    Serial.println(rychlost);
    Serial.print("MAX rychlost");
    Serial.println(MAX_rychlost);
  }
}