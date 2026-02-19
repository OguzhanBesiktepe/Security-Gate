#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// ==================== PINS ====================
const int BUZZER = 8;
const int GREEN_LED = 6;
const int RED_LED = 7;
const int SERVO_PIN = 9;

// RC522
#define SS_PIN 53
#define RST_PIN 5

// LCD1602 parallel (4-bit): RS, E, D4, D5, D6, D7
LiquidCrystal lcd(40, 41, 42, 43, 44, 45);

// Keypad pins (you already wired these)
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ==================== OBJECTS ====================
Servo gateServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ==================== CONFIG ====================
static const char *ADMIN_PIN = "9999"; // change if you want

// EEPROM layout
static const uint32_t EEPROM_MAGIC = 0x53473131; // "SG11"
static const int EEPROM_MAGIC_ADDR = 0;

// How many users to store
static const uint8_t MAX_USERS = 50;

// Each user: active + uidLen + uid(7) + pin(4)  => 13 bytes packed
struct __attribute__((packed)) UserRecord
{
    uint8_t active; // 0xA5 = active, 0x00 = empty
    uint8_t uidLen; // 4 or 7
    uint8_t uid[7]; // store up to 7 bytes
    char pin[4];    // exactly 4 digits, no null terminator
};

static const int RECORD_SIZE = sizeof(UserRecord);
static const int EEPROM_USERS_ADDR = EEPROM_MAGIC_ADDR + (int)sizeof(uint32_t);

// ==================== STATE ====================
enum State
{
    WAIT_PIN,
    WAIT_RFID,
    ADMIN_SCAN_UID,
    ADMIN_ENTER_PIN
};

State state = WAIT_PIN;

String enteredPin = "";
String pendingPin = "";  // used in admin enter-pin step
MFRC522::Uid pendingUid; // last scanned UID (for enrollment)

// ==================== UI / HELPERS ====================
void gateClose() { gateServo.write(0); }
void gateOpen() { gateServo.write(90); }

void keyBeep() { tone(BUZZER, 1200, 50); } // keypress beep

void lcdClearLine(int row)
{
    lcd.setCursor(0, row);
    lcd.print("                ");
}

void lcdShowEnterPin()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN:");
    lcd.setCursor(0, 1);
    lcd.print("PIN: ");
    lcd.print(enteredPin); // show digits for demo
}

void lcdShowScanRFID()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan RFID...");
    lcd.setCursor(0, 1);
    lcd.print("Then wait");
}

void lcdShowSuccess()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Welcome In!");
}

void lcdShowFailure()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
}

void lcdShowAdminScan()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ADMIN MODE");
    lcd.setCursor(0, 1);
    lcd.print("Scan card...");
}

void lcdShowAdminEnterPin()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set user PIN:");
    lcd.setCursor(0, 1);
    lcd.print("PIN: ");
    lcd.print(pendingPin);
}

void toneSuccess()
{
    tone(BUZZER, 1800, 150);
    delay(200);
    tone(BUZZER, 2200, 150);
    delay(200);
}

void toneFailure()
{
    tone(BUZZER, 350, 500);
    delay(550);
}

void showTempMessage(const char *line1, const char *line2, int ms = 900)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    delay(ms);
}

// ==================== EEPROM HELPERS ====================
int recordAddr(uint8_t index)
{
    return EEPROM_USERS_ADDR + (index * RECORD_SIZE);
}

UserRecord readRecord(uint8_t index)
{
    UserRecord r;
    EEPROM.get(recordAddr(index), r);
    return r;
}

void writeRecord(uint8_t index, const UserRecord &r)
{
    EEPROM.put(recordAddr(index), r);
}

bool uidEquals(const uint8_t *a, uint8_t aLen, const uint8_t *b, uint8_t bLen)
{
    if (aLen != bLen)
        return false;
    for (uint8_t i = 0; i < aLen; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

bool pinEquals4(const char *stored4, const String &entered)
{
    if (entered.length() != 4)
        return false;
    for (int i = 0; i < 4; i++)
        if (stored4[i] != entered[i])
            return false;
    return true;
}

int findUserByUid(const MFRC522::Uid &uid)
{
    for (uint8_t i = 0; i < MAX_USERS; i++)
    {
        UserRecord r = readRecord(i);
        if (r.active != 0xA5)
            continue;
        if (uidEquals(r.uid, r.uidLen, uid.uidByte, uid.size))
            return i;
    }
    return -1;
}

int findEmptySlot()
{
    for (uint8_t i = 0; i < MAX_USERS; i++)
    {
        UserRecord r = readRecord(i);
        if (r.active != 0xA5)
            return i;
    }
    return -1;
}

void eepromInitIfNeeded()
{
    uint32_t magic = 0;
    EEPROM.get(EEPROM_MAGIC_ADDR, magic);

    if (magic == EEPROM_MAGIC)
        return;

    // initialize
    EEPROM.put(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
    UserRecord empty{};
    empty.active = 0x00;
    empty.uidLen = 0;
    for (uint8_t i = 0; i < 7; i++)
        empty.uid[i] = 0;
    for (uint8_t i = 0; i < 4; i++)
        empty.pin[i] = '0';

    for (uint8_t i = 0; i < MAX_USERS; i++)
        writeRecord(i, empty);
}

// Save/update user record for pendingUid + pendingPin
bool upsertUser(const MFRC522::Uid &uid, const String &pin4)
{
    if (uid.size != 4 && uid.size != 7)
        return false;
    if (pin4.length() != 4)
        return false;

    int idx = findUserByUid(uid);
    if (idx < 0)
        idx = findEmptySlot();
    if (idx < 0)
        return false; // full

    UserRecord r{};
    r.active = 0xA5;
    r.uidLen = uid.size;
    for (uint8_t i = 0; i < 7; i++)
        r.uid[i] = 0;
    for (uint8_t i = 0; i < uid.size; i++)
        r.uid[i] = uid.uidByte[i];
    for (uint8_t i = 0; i < 4; i++)
        r.pin[i] = pin4[i];

    writeRecord((uint8_t)idx, r);
    return true;
}

void resetToPinEntry()
{
    enteredPin = "";
    pendingPin = "";
    state = WAIT_PIN;

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    gateClose();

    lcdShowEnterPin();
}

void doSuccess()
{
    lcdShowSuccess();
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    toneSuccess();
    gateOpen();
    delay(1500);
    gateClose();
    digitalWrite(GREEN_LED, LOW);
    delay(600);
}

void doFailure()
{
    lcdShowFailure();
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    toneFailure();
    digitalWrite(RED_LED, LOW);
    gateClose();
    delay(600);
}

char getKeyWithBeep()
{
    char k = keypad.getKey();
    if (k)
        keyBeep();
    return k;
}

void setup()
{
    pinMode(BUZZER, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    gateServo.attach(SERVO_PIN);
    gateClose();

    lcd.begin(16, 2);
    showTempMessage("System Starting", "Please wait...", 800);

    // RFID init
    pinMode(53, OUTPUT);
    digitalWrite(53, HIGH);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(5);

    // EEPROM init
    eepromInitIfNeeded();

    resetToPinEntry();
}

void loop()
{
    // -------- WAIT_PIN --------
    if (state == WAIT_PIN)
    {
        char k = getKeyWithBeep();
        if (!k)
            return;

        if (k == '*')
        {
            enteredPin = "";
            lcdShowEnterPin();
            return;
        }

        if (k == '#')
        {
            // Require exactly 4 digits
            if (enteredPin.length() != 4)
            {
                doFailure();
                resetToPinEntry();
                return;
            }

            // Admin entry?
            if (enteredPin == ADMIN_PIN)
            {
                enteredPin = "";
                state = ADMIN_SCAN_UID;
                lcdShowAdminScan();
                return;
            }

            // Normal user: go scan RFID
            state = WAIT_RFID;
            lcdShowScanRFID();
            return;
        }

        // Digits only
        if (k >= '0' && k <= '9')
        {
            if (enteredPin.length() < 4)
            {
                enteredPin += k;
                lcdShowEnterPin();
            }
            else
            {
                tone(BUZZER, 400, 60);
            }
        }
        // Ignore A/B/C/D silently
        return;
    }

    // -------- WAIT_RFID (paired auth) --------
    if (state == WAIT_RFID)
    {
        if (!mfrc522.PICC_IsNewCardPresent())
            return;
        if (!mfrc522.PICC_ReadCardSerial())
            return;

        int idx = findUserByUid(mfrc522.uid);
        bool ok = false;
        if (idx >= 0)
        {
            UserRecord r = readRecord((uint8_t)idx);
            ok = pinEquals4(r.pin, enteredPin); // paired check
        }

        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        if (ok)
            doSuccess();
        else
            doFailure();

        resetToPinEntry();
        return;
    }

    // -------- ADMIN_SCAN_UID --------
    if (state == ADMIN_SCAN_UID)
    {
        // Allow cancel with *
        char k = keypad.getKey();
        if (k == '*')
        {
            keyBeep();
            resetToPinEntry();
            return;
        }

        if (!mfrc522.PICC_IsNewCardPresent())
            return;
        if (!mfrc522.PICC_ReadCardSerial())
            return;

        // store pending UID
        pendingUid.size = mfrc522.uid.size;
        for (byte i = 0; i < pendingUid.size; i++)
            pendingUid.uidByte[i] = mfrc522.uid.uidByte[i];

        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        pendingPin = "";
        state = ADMIN_ENTER_PIN;
        lcdShowAdminEnterPin();
        return;
    }

    // -------- ADMIN_ENTER_PIN --------
    if (state == ADMIN_ENTER_PIN)
    {
        char k = getKeyWithBeep();
        if (!k)
            return;

        if (k == '*')
        {
            pendingPin = "";
            lcdShowAdminEnterPin();
            return;
        }

        if (k == '#')
        {
            if (pendingPin.length() != 4)
            {
                showTempMessage("Need 4 digits", "Try again", 900);
                pendingPin = "";
                lcdShowAdminEnterPin();
                return;
            }

            bool saved = upsertUser(pendingUid, pendingPin);
            if (saved)
                showTempMessage("User Saved", "EEPROM updated", 1000);
            else
                showTempMessage("Save Failed", "EEPROM full?", 1200);

            resetToPinEntry();
            return;
        }

        if (k >= '0' && k <= '9')
        {
            if (pendingPin.length() < 4)
            {
                pendingPin += k;
                lcdShowAdminEnterPin();
            }
            else
            {
                tone(BUZZER, 400, 60);
            }
        }
        return;
    }
}