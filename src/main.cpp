#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// Digital Pins
const int BUZZER = 8;
const int GREEN_LED = 6;
const int RED_LED = 7;
const int SERVO_PIN = 9;

// RC522 RFID Module
#define SS_PIN 53
#define RST_PIN 5

// LCD1602 Display (parallel, 4-bit): RS, E, D4, D5, D6, D7
LiquidCrystal lcd(40, 41, 42, 43, 44, 45);

// Servo Motor for gate/door control
Servo gateServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Keypad setup (4x4 matrix)
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Digital Pin layout for keypad rows and columns
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// -------------------- Credentials --------------------
const char *VALID_PIN = "1234"; // demo PIN

// Your allowed RFID UID: A5 81 AA 04
const byte ALLOWED_UID[][10] = {
    {0xA5, 0x81, 0xAA, 0x04}};
const byte ALLOWED_UID_LEN[] = {4};

// Waiting States
enum State
{
    WAIT_PIN,
    WAIT_RFID
};
State state = WAIT_PIN;

String enteredPin = "";

// Servo control functions
void gateClose() { gateServo.write(0); }
void gateOpen() { gateServo.write(90); }

// function for beeper feedback on key press, success, and failure
void keyBeep()
{
    tone(BUZZER, 1200, 60); // short 60ms beep
}

void lcdPromptEnterPin() // LCD prompt for entering PIN, also shows current digits entered
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN:");
    lcd.setCursor(0, 1);
    lcd.print("PIN: ");
    // show current digits
    lcd.print(enteredPin);
}

void lcdShowScanRFID() // LCD prompt for scanning RFID after correct PIN
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PIN OK");
    lcd.setCursor(0, 1);
    lcd.print("Scan RFID...");
}

void lcdShowSuccess() // LCD message for successful access
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Welcome In!");
}

void lcdShowFailure() // LCD message for failed access
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
}

void resetFlow()
{
    enteredPin = "";
    state = WAIT_PIN;

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    gateClose();

    lcdPromptEnterPin();
}

void success()
{
    lcdShowSuccess();

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
    delay(600); // keep message visible briefly
}

void failure()
{
    lcdShowFailure();

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    // Lower pitch failure tone
    tone(BUZZER, 350, 500);
    delay(550);

    digitalWrite(RED_LED, LOW);
    gateClose();
    delay(600); // keep message visible briefly
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

void setup()
{
    // Outputs
    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    // Servo
    gateServo.attach(SERVO_PIN);
    gateClose();

    // LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Starting");
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");
    delay(800);

    // RFID
    pinMode(53, OUTPUT); // Mega SPI stability
    digitalWrite(53, HIGH);

    SPI.begin();
    mfrc522.PCD_Init();
    delay(5);

    resetFlow();
}

void loop()
{
    // User Enters Pin:
    if (state == WAIT_PIN)
    {
        char k = keypad.getKey();
        if (!k)
            return;

        keyBeep();

        if (k == '*')
        {
            enteredPin = "";
            lcdPromptEnterPin();
            return;
        }

        if (k == '#')
        {
            // Must be exactly 4 digits
            if (enteredPin.length() != 4)
            {
                failure();
                resetFlow();
                return;
            }

            if (enteredPin != VALID_PIN)
            {
                failure();
                resetFlow();
                return;
            }

            // PIN is correct
            state = WAIT_RFID;
            lcdShowScanRFID();
            return;
        }

        // Only allow digits, max 4
        if (k >= '0' && k <= '9')
        {
            if (enteredPin.length() < 4)
            {
                enteredPin += k;
                lcdPromptEnterPin();
            }
            else
            {
                // Already have 4 digits, ignore extra input but give a warning beep
                tone(BUZZER, 400, 60);
            }
        }
        // Ignore A/B/C/D on keypad
        return;
    }

    // Step 2: Scan RFID
    if (!mfrc522.PICC_IsNewCardPresent())
        return;
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    bool rfidOk = uidMatchesAllowed(mfrc522.uid);

    if (rfidOk)
        success();
    else
        failure();

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    resetFlow();
}