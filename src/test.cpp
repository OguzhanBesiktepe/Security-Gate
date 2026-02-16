#include <Arduino.h>

// testing components

// buzzer connected to pin 8
//  will produce a 1000 Hz tone for 300 ms, then be silent for 700 ms, on repeat

// green LED connected to pin 6
// red LED connected to pin 7

// both LEDS will be on

const int BUZZER = 8;
const int GREEN_LED = 6;
const int RED_LED = 7;

void setup()
{
    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
}

void loop()
{

    digitalWrite(GREEN_LED, HIGH); // turn on green LED
    digitalWrite(RED_LED, HIGH);   // turn on red LED

    tone(BUZZER, 1000); // 1000 Hz tone
    delay(300);

    noTone(BUZZER);
    delay(700);
}