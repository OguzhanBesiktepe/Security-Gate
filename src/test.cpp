#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>

// -------------------- Pins --------------------
const int BUZZER = 8;
const int GREEN_LED = 6;
const int RED_LED = 7;
const int SERVO_PIN = 9;

#define SS_PIN 53
#define RST_PIN 5

// -------------------- Objects --------------------
Servo gateServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

// -------------------- Keypad (4x4) --------------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Ignoring A/B/C/D since we only want digits and * for clear, # for submit

// Digital Pins for rows and columns of the keypad

// Keypad has 4 rows connected to digital pins:
byte rowPins[4] = {22, 24, 26, 28};

// Keypad has 4 columns connected to digital pins:
byte colPins[4] = {30, 32, 34, 36};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Test Valid Code:

// 4-digit PIN only
const char *VALID_PIN = "1234";

// Allowed RFID UID(s)
// Example UID print: UID: 3A 9F 21 7C  ->  {0x3A,0x9F,0x21,0x7C}

// UID prints = A5 81 AA 04

// Need to change to C++ hex format

const byte ALLOWED_UID[][10] = {
    {0xA5, 0x81, 0xAA, 0x04}};
const byte ALLOWED_UID_LEN[] = {4};

// -------------------- State --------------------
enum State
{
    WAIT_PIN,
    WAIT_RFID
};
State state = WAIT_PIN;

String enteredPin = "";

// -------------------- Helpers --------------------
void gateClose() { gateServo.write(0); }
void gateOpen() { gateServo.write(90); }

void resetFlow()
{
    enteredPin = "";
    state = WAIT_PIN;
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    gateClose();

    Serial.println("Enter 4-digit PIN, then press #. (* clears)");
}

void success()
{
    Serial.println("Correct Credentials, Welcome In!");
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    // Higher pitch success tone
    tone(BUZZER, 1800, 150);
    delay(200);
    tone(BUZZER, 2200, 150);
    delay(200);

    gateOpen();
    delay(1500);
    gateClose();

    digitalWrite(GREEN_LED, LOW);
}

void failure()
{
    Serial.println("Incorrect credentials, Please Try Again");
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    // Lower pitch failure tone
    tone(BUZZER, 350, 500);
    delay(550);

    digitalWrite(RED_LED, LOW);
    gateClose(); // remain closed
}

bool uidMatchesAllowed(const MFRC522::Uid &uid)
{
    for (size_t i = 0; i < (sizeof(ALLOWED_UID_LEN) / sizeof(ALLOWED_UID_LEN[0])); i++)
    {
        if (uid.size != ALLOWED_UID_LEN[i])
            continue;

        bool match = true;
        for (byte j = 0; j < uid.size; j++)
        {
            if (uid.uidByte[j] != ALLOWED_UID[i][j])
            {
                match = false;
                break;
            }
        }
        if (match)
            return true;
    }
    return false;
}

void printUid(const MFRC522::Uid &uid)
{
    Serial.print("UID:");
    for (byte i = 0; i < uid.size; i++)
    {
        Serial.print(uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(uid.uidByte[i], HEX);
    }
    Serial.println();
}

void setup()
{
    Serial.begin(9600);

    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    gateServo.attach(SERVO_PIN);
    gateClose();

    // Mega SPI stability: SS must be OUTPUT
    pinMode(53, OUTPUT);
    digitalWrite(53, HIGH);

    SPI.begin();
    mfrc522.PCD_Init();
    delay(5);

    Serial.println("System ready.");
    Serial.println("Enter 4-digit PIN, then press #. (* clears)");
    mfrc522.PCD_DumpVersionToSerial();
}

void loop()
{
    // First Pin Entry
    if (state == WAIT_PIN)
    {
        char k = keypad.getKey();
        if (!k)
            return;

        Serial.print("Key: ");
        Serial.println(k);

        if (k == '*')
        {
            enteredPin = "";
            Serial.println("PIN cleared");
            Serial.println("Enter 4-digit PIN, then press #. (* clears)");
            return;
        }

        if (k == '#')
        {
            // Must be exactly 4 digits
            if (enteredPin.length() != 4)
            {
                Serial.println("PIN must be exactly 4 digits.");
                failure();
                resetFlow();
                return;
            }

            if (enteredPin != VALID_PIN)
            {
                Serial.println("PIN incorrect.");
                failure();
                resetFlow();
                return;
            }

            Serial.println("PIN OK. Now scan RFID card/tag...");
            state = WAIT_RFID;
            return;
        }

        // Only accept digits
        if (k >= '0' && k <= '9')
        {
            if (enteredPin.length() < 4)
            {
                enteredPin += k;

                // Will mask the actual digits for privacy, but show progress with *
                Serial.print("PIN entered: ");
                for (int i = 0; i < enteredPin.length(); i++)
                    Serial.print('*');
                Serial.print(" (");
                Serial.print(enteredPin.length());
                Serial.println("/4)");
            }
            else
            {
                // Ignore extra digits; optional beep
                Serial.println("Already have 4 digits. Press # to submit or * to clear.");
                tone(BUZZER, 400, 60);
            }
        }
        else
        {
            // Ignore A/B/C/D inputs
            Serial.println("Non-digit ignored (A/B/C/D).");
        }

        return;
    }

    // Second RFID Check
    if (!mfrc522.PICC_IsNewCardPresent())
        return;
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    printUid(mfrc522.uid);

    bool rfidOk = uidMatchesAllowed(mfrc522.uid);

    if (rfidOk)
        success();
    else
        failure();

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    resetFlow();
}