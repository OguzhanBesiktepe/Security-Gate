🛡️ Embedded Access Control System (RFID + PIN)

A multi-factor embedded security system built on the Arduino Mega that restricts access using a paired 4-digit PIN and RFID card (UID).

The system features EEPROM-backed user enrollment, LCD-based UI prompts, and physical gate actuation using a servo motor.

This project combines hardware integration, persistent storage, and structured state-machine firmware design in C++.

📹 Demo Video

(Insert your YouTube link here)

🔐 System Overview

User enters a 4-digit PIN via keypad

User scans RFID card

System verifies paired credentials from EEPROM

If valid:

Green LED

Success tone

Servo opens gate

If invalid:

Red LED

Failure tone

Gate remains closed

🛠 Admin Mode

Enter Admin PIN (9999)

Create new users:

Enter 4-digit PIN

Scan RFID card

Credentials stored in EEPROM

Supports up to 50 users

Credentials persist after power cycle

📐 Hardware Used

Arduino Mega 2560

MFRC522 RFID Reader (SPI)

4x4 Matrix Keypad

LCD1602 (Parallel, 4-bit mode)

Servo Motor

Passive Buzzer

Red & Green LEDs

10k Potentiometer (LCD contrast)

Breadboard + jumper wires

💻 Software Details

Written in C++ (Arduino framework)

Built with PlatformIO

State-machine driven logic

EEPROM structured user records

SPI communication for RFID

Parallel 4-bit LCD interface

Non-blocking keypress feedback

🧠 Key Concepts Demonstrated

Multi-factor authentication logic

Paired credential verification (UID ↔ PIN)

EEPROM persistent storage

Structured state-machine design

Multi-peripheral embedded integration

Hardware debugging and validation

🚀 Future Improvements

Failed-attempt lockout

Admin user deletion

Event logging

RTC integration

3D printed enclosure

Secure RFID (DESFire)
