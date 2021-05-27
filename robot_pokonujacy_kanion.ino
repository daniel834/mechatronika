#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
  
#define dirA 2
#define dirB 3
#define dirC 4
#define stepA 5
#define stepB 6
#define stepC 7
#define enable 8
#define stopB 9
#define stopC 10
#define start1 11
#define trigPin 13
#define echoPin 12

LiquidCrystal_I2C lcd(0x3f,16,2);  // ustaw adres lcd na 0x3f dla wyświetlacza 16 kolumn na 2 wiersze

bool START, stanB, stanC, kraw, brakkraw, kontynuuj;
unsigned long p = 5;  // długość przerwy między krokami 
long czas, dystans;
int  kroki, i, pozMiejsce, j, kroki1, k, l;
float kanion;
const int maxkan = 500;  // maksymalna długość przerwy [mm]
const int dlmost = 700;  //maksymalne wysunięcie mostu [mm]
const int wys = 200;  // wysokość podnoszenia mostu [ilość kroków]

void setup() {

  pinMode(dirA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(dirC, OUTPUT);
  pinMode(stepA, OUTPUT);
  pinMode(stepB, OUTPUT);
  pinMode(stepC, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(stopB, INPUT_PULLUP);
  pinMode(stopC, INPUT_PULLUP);
  pinMode(start1, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT); //Pin, do którego podłączymy trig jako wyjście
  pinMode(echoPin, INPUT); //a echo, jako wejście
  pinMode(A3, INPUT);

  lcd.init();                      // initialize the lcd 
  lcd.backlight(); 

  START = HIGH;
  stanB = HIGH;
  stanC = HIGH;
}


void loop() {

  kraw = HIGH;
  kroki = 0;
  brakkraw = LOW;
 
  digitalWrite(enable, HIGH);
if (kontynuuj == LOW){
  lcd.setCursor(0,0);
  lcd.print("Aby rozpoczac");
  lcd.setCursor(0,1);
  lcd.print("nacisnij start");

  while (START == HIGH ) {
    START = digitalRead(start1);
    delay(10);
  }
}


  if (kontynuuj == HIGH){
    lcd.clear();
    lcd.home();
    lcd.print("wcisnij start");
    lcd.setCursor(0,1);
    lcd.print("aby jechac dalej");
    while (START == HIGH ) {
      START = digitalRead(start1);
      delay(10);
  }
  }

  digitalWrite(enable, HIGH);

  lcd.clear();
  lcd.home();
  lcd.print("Ustaw poz start");

  while (stanB == HIGH) {  // podnoszenie mostu

    digitalWrite(enable, LOW);
    digitalWrite(dirB, HIGH);
    digitalWrite(stepB, HIGH);
    delay(p);
    digitalWrite(stepB, LOW);
    delay(p);
    stanB = digitalRead(stopB);
  }

  while (stanC == HIGH) {  // cofanie mostu

    digitalWrite(enable, LOW);
    digitalWrite(dirC, LOW);
    digitalWrite(stepC, HIGH);  
    delay(5);                   
    digitalWrite(stepC, LOW);
    delay(5);
    stanC = digitalRead(stopC);
  }

  lcd.clear();
  lcd.home();
  lcd.print("Szukanie kraw");

  while (dystans < 20) {
    
    digitalWrite(enable, LOW);
    digitalWrite(trigPin, LOW);  // mierzenie odległośći od ziemi
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    czas = pulseIn(echoPin, HIGH);
    dystans = czas / 58;

    digitalWrite(dirA, HIGH);  // kierunek obrotu 
    digitalWrite(stepA, HIGH);  //wykonaj 1 krok
    delay(p);
    digitalWrite(stepA, LOW);
    delay(p);

    if (dystans > 100){    // jeśli podłoga znajduje się w martwym polu czujnika zwraca on wartość maksymalną
      dystans = 1;              
    }                           
  }

  lcd.clear();                     // woda mineralna
  lcd.home();                      // to taki kompot z kamieni
  lcd.print("Wysowanie mostu");    // :)

  while (kraw == HIGH) {

    kraw = digitalRead(A3);   // czujnik na moście zwraca wartość 1 jeśli wykryje przeszkodę
    digitalWrite(dirC, HIGH);
    digitalWrite(stepC, HIGH);
    delay(p);
    digitalWrite(stepC, LOW);
    delay(p);
    kroki++ ;  // liczenie kroków
    kanion = kroki * 0.942; // przeliczenie na mm
    l++;

    if (kanion > dlmost){   // zabezpieczenie na wypadek nie wykrycia krawędzi
      kraw = LOW;
      brakkraw = HIGH;   // czujnik nie wykrył krawędzi
    }

  }

  pozMiejsce = dlmost - (int)kanion;  // pozostałe miejsce 
  delay(1000);

switch (brakkraw) 
{
  case HIGH:      // jeśli czujnik nie wykrył krawędzi
    anulowanie();
  break;

  case LOW:   // jeżeli czujnik wykrył krawędź

    lcd.clear();
    lcd.home();
    lcd.print("Wysow mostu cd");

    kroki1 = pozMiejsce / 0.942 ;

    for (j = 0; j < kroki1; j++) {   // wysuwa pozostałą część mostu
      digitalWrite(dirC, HIGH);
      digitalWrite(stepC, HIGH);
      delay(p);
      digitalWrite(stepC, LOW);
      delay(p);
    }

    lcd.clear();
    lcd.home();
    lcd.print("opuszczanie");

    for (k = 0; k < wys; k++) {   //  opuszcza robota
      digitalWrite(dirB, LOW);
      digitalWrite(stepB, HIGH);
      delay(p);
      digitalWrite(stepB, LOW);
      delay(p);   
    }

    kontynuuj = HIGH;

  break;
  
}
  START = HIGH;
  stanB = HIGH;
  stanC = HIGH;
  dystans = 0;

}

void anulowanie(){

  lcd.clear();
  lcd.home();
  lcd.print("za duza przerwa");
  delay(2000);
  lcd.clear();
  lcd.home();
  lcd.print("Anulowanie");

  digitalWrite(enable, LOW);

  for (i = 0 ; i < 200; i++) { // cofanie robota
    digitalWrite(dirA, LOW);
    digitalWrite(stepA, HIGH);
    delay(p);
    digitalWrite(stepA, LOW);
    delay(p);
  }
  stanC = HIGH;

  while (stanC == HIGH) {  // cofanie mostu
    digitalWrite(dirC, LOW);
    digitalWrite(stepC, HIGH);
    delay(p);
    digitalWrite(stepC, LOW);
    delay(p);
    stanC = digitalRead(stopC);
  }
  START = HIGH;
  kontynuuj = LOW;
}
