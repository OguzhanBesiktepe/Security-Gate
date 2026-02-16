#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// -------------------- Pin Definitions --------------------
const int BUZZER = 8;
const int GREEN_LED = 6;
const int RED_LED = 7;
const int SERVO_PIN = 9;

#define SS_PIN 53 // RC522 SDA/SS
#define RST_PIN 5 // RC522 RST

// -------------------- Objects --------------------
Servo gateServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

// -------------------- Timing (non-blocking) --------------------
unsigned long lastHeartbeat = 0;
bool heartbeatState = false;

// -------------------- Setup --------------------
void setup()
{
    Serial.begin(9600);

    // Mega SPI stability requirement
    pinMode(53, OUTPUT);
    digitalWrite(53, HIGH);

    // Outputs
    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    gateServo.attach(SERVO_PIN);
    gateServo.write(0); // start "closed"

    // RFID init
    SPI.begin();
    mfrc522.PCD_Init();
    delay(5);

    Serial.println("RFID + outputs test running...");
    mfrc522.PCD_DumpVersionToSerial();
    Serial.println("Tap a card/tag to print UID and move servo.");
}

// -------------------- Heartbeat (LED + buzzer) --------------------
void heartbeat()
{
    // Toggle every 500ms without blocking RFID reading
    unsigned long now = millis();
    if (now - lastHeartbeat >= 500)
    {
        lastHeartbeat = now;
        heartbeatState = !heartbeatState;

        // Alternate LEDs
        digitalWrite(GREEN_LED, heartbeatState ? HIGH : LOW);
        digitalWrite(RED_LED, heartbeatState ? LOW : HIGH);

        // Short beep when state turns ON
        if (heartbeatState)
        {
            tone(BUZZER, 1000, 80); // 80ms beep (non-blocking)
        }
    }
}

// -------------------- Main Loop --------------------
void loop()
{
    // Keep showing signs of life
    heartbeat();

    // RFID read (non-blocking)
    if (!mfrc522.PICC_IsNewCardPresent())
        return;
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    // Print UID
    Serial.print("UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Feedback: quick double-beep + servo open/close
    tone(BUZZER, 1500, 120);
    delay(150);
    tone(BUZZER, 1500, 120);

    gateServo.write(90); // "open"
    delay(700);
    gateServo.write(0); // "close"

    // Stop reading this card
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Small debounce so it doesn't spam reads on one tap
    delay(300);
}
