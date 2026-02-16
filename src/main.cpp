#include <Arduino.h>

// testing components

// buzzer connected to pin 8
//  will produce a 1000 Hz tone for 300 ms, then be silent for 700 ms, on repeat

const int BUZZER = 8;

void setup()
{
  pinMode(BUZZER, OUTPUT);
}

void loop()
{
  tone(BUZZER, 1000); // 1000 Hz tone
  delay(300);

  noTone(BUZZER);
  delay(700);
}