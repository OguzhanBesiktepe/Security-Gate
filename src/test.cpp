#include <Arduino.h>
#include <servo.h>

// testing components

// buzzer connected to pin 8
//  will produce a 1000 Hz tone for 300 ms, then be silent for 700 ms, on repeat

const int BUZZER = 8;

// green LED connected to pin 6
// red LED connected to pin 7

const int GREEN_LED = 6;
const int RED_LED = 7;

// servo motor connected to pin 9

const int SERVO_PIN = 9;
Servo gateServo;

void setup()
{
    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    gateServo.attach(SERVO_PIN);
}

void loop()
{
    digitalWrite(GREEN_LED, HIGH); // turn on green LED
    digitalWrite(RED_LED, HIGH);   // turn on red LED

    tone(BUZZER, 1000); // 1000 Hz tone
    delay(300);

    noTone(BUZZER);
    delay(700);

    gateServo.write(90); // move servo to 90 degrees
    delay(1000);
    gateServo.write(0); // move servo back to 0 degrees
    delay(1000);
}