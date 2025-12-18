#include <Wire.h>
#include <RTClib.h>

// Pins für den Treiber
const int stepPin   = 2;
const int dirPin    = 3;
const int enablePin = 4;

RTC_DS3231 rtc;

// Hub
const long STEPS_MOVE = 8000;

// Richtung (einmal festlegen und dann bei Bedarf tauschen)
const bool DIR_UP   = HIGH;
const bool DIR_DOWN = LOW;

// Damit es pro Tag nur einmal auslöst
int lastDayUp   = -1;
int lastDayDown = -1;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin,  OUTPUT);
  pinMode(enablePin, OUTPUT);

  // Motor am Anfang AUS (kein Pfeifen)
  digitalWrite(enablePin, HIGH);

  Wire.begin();

  if (!rtc.begin()) {
    while (1) { }
  }

  // ***** UHR EINMALIG STELLEN *****
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // Danach wieder auskommentieren!
}

void loop() {
  DateTime now = rtc.now();

  // 06:00 -> hoch
  if (now.hour() == 6 && now.minute() == 0) {
    if (now.day() != lastDayUp) {
      lastDayUp = now.day();
      moveSteps(STEPS_MOVE, DIR_UP);
    }
  }

  // 20:00 -> runter
  if (now.hour() == 20 && now.minute() == 0) {
    if (now.day() != lastDayDown) {
      lastDayDown = now.day();
      moveSteps(STEPS_MOVE, DIR_DOWN);
    }
  }

  // Motor bleibt sonst aus
  delay(500); // entlastet CPU, Zeit kommt ja aus der RTC
}

void moveSteps(long steps, bool direction) {
  // Motor EIN
  digitalWrite(enablePin, LOW);

  // Richtung setzen
  digitalWrite(dirPin, direction);

  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }

  // Motor wieder AUS (kein Pfeifen)
  digitalWrite(enablePin, HIGH);
}
