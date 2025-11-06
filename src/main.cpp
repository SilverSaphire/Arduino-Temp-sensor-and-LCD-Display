#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal.h>
#include <Chrono.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11
#define BLUE_LED 11
#define GREEN_LED 12
#define RED_LED 13
#define BUZZER_PIN 3
#define MyButton 8
int Button_Status = 0;

#define HIGH_TEMP_THRESHOLD 75
#define LOW_TEMP_THRESHOLD 70
#define TONE_HIGH 1000

//initialize DHT sensor
DHT dht(DHT_PIN, DHT11);
LiquidCrystal lcd (10,9,7,6,5,4);

int buzzerState = 0;
int Button_Held = 0;
int Button_Press = 0;

Chrono chronoA;
Chrono chronoB;

void playingHighTempAlert();

void setup() {
  // put your setup code here, to run once:
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(MyButton, INPUT);
  Serial.begin(9600);
  Serial.println("Temp monitoring system active");
  dht.begin();
  lcd.begin(16,2);
  lcd.setCursor (0,0);
  lcd.print("Temp monitoring");
}

void TempRead() {
  float temperature = dht.readTemperature(true);
  Serial.println(temperature);
  if (temperature > 80) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    if (Button_Held == 0) {
      buzzerState = 1;
      tone(BUZZER_PIN, TONE_HIGH);
    }
  }
  else if (temperature < 75) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
    noTone(BUZZER_PIN);
    buzzerState = 0;
  }
  else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
    noTone(BUZZER_PIN);
    buzzerState = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  float temperature = dht.readTemperature(true);
  lcd.setCursor(0,1);
  lcd.print(String(temperature));
  if (isnan(temperature)) {
    Serial.println("Failed to read DHT sensor");
    lcd.print("Failed to read DHT sensor");
    return;
  }
  Button_Status = digitalRead(MyButton);
  if (Button_Status == HIGH) {
    chronoA.restart();
  }
  //loop
  while (Button_Status == HIGH) {
    if(chronoA.hasPassed(2000)) {
      Button_Status = digitalRead(MyButton);
      delay(500);
      //Serial.println(chronoA);
      chronoB.restart();
    }
    if (chronoB.hasPassed(3000)) {
      buzzerState = 0; 
      Button_Held = 1;
      break;
    }
  }
  TempRead;
  Serial.println("Logic");
  Serial.println(Button_Held == 1 and Button_Status == 1);
  //reset
  if (Button_Held == 1 and Button_Status == 1) {
    Serial.println("Hit");
    Button_Held = 0;
  }
}

//Stuck on an issue where my reset activates before my loop
//if button is held for 5 seconds buzzer stays off
//if button is pressed again, buzzer turns back again