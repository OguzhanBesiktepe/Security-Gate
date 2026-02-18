🛡️ Embedded Access Control System
RFID + PIN Authentication with EEPROM Enrollment (Arduino Mega)

🧩 Problem

Unauthorized individuals are gaining access to restricted areas due to insufficient authentication mechanisms (e.g., physical keys, shared access codes, or single-factor authentication systems).

There is a need for a low-cost, embedded access control solution that:

Restricts entry to authorized users only

Prevents shared credential misuse

Supports user enrollment and removal

Persists credentials after power loss

💡 Solution

Design and implement a multi-factor embedded security system that requires:

A 4-digit PIN

A paired RFID card/tag (UID)

Both credentials must correspond to a stored user record in EEPROM for access to be granted.

An Admin Mode enables secure user enrollment and credential management.

📌 Overview

This project is a fully functional embedded access control system built using an Arduino Mega 2560.

The system integrates:

Keypad input

RFID authentication

LCD user interface

Servo-based gate actuation

EEPROM persistent storage

State-machine firmware architecture

🔐 Features
👤 Multi-Factor Authentication

4-digit PIN entry via keypad

RFID scan required after PIN submission

Paired authentication (UID ↔ PIN must match)

Supports 4-byte and 7-byte RFID UIDs

🛠 Admin Mode (EEPROM Enrollment)

Enter Admin PIN (9999)

Register new users:

Enter new 4-digit PIN

Scan RFID card

Credentials saved to EEPROM

Supports up to 50 stored users

Credentials persist after power loss

🖥 Embedded User Interface

LCD1602 (Parallel 16x2 display)

Real-time prompts:

“Enter PIN”

“Scan RFID”

“Access Granted”

“Access Denied”

Keypress audio feedback

Success and failure tones

🚪 Physical Feedback

Servo motor simulates gate/door opening

Green LED → Access granted

Red LED → Access denied

Passive buzzer → Audio feedback

🧠 System Architecture
State Machine Design

Authentication flow:

WAIT_PIN
   → WAIT_RFID
       → SUCCESS / FAILURE


Admin enrollment flow:

WAIT_PIN (Admin PIN)
   → ADMIN_ENTER_PIN
       → ADMIN_SCAN_UID
           → EEPROM Write


This structured design ensures:

Clean state transitions

Deterministic behavior

Expandability for future features

💾 EEPROM Data Structure

Each stored user record contains:

Active flag

UID length

UID bytes (up to 7 bytes)

4-digit PIN

A magic header is used to initialize EEPROM once and prevent accidental overwrites.

Credentials remain stored after power cycles.

🧰 Hardware Required

Arduino Mega 2560

MFRC522 RFID Reader (13.56 MHz)

RFID cards / key fobs

4x4 Matrix Keypad

LCD1602 (Parallel version)

10k Potentiometer (contrast control)

Servo motor

Passive buzzer

Red LED + Green LED

220Ω resistors

Breadboard + jumper wires

External 5V supply recommended for servo

🔌 Communication Interfaces Used

SPI (RFID module)

Parallel 4-bit LCD interface

Matrix keypad scanning

PWM (Servo control)

EEPROM persistent storage

📚 What This Project Demonstrates

Embedded firmware development

Multi-peripheral hardware integration

EEPROM persistent data management

Authentication system design

State machine architecture

Secure design thinking

Real-world hardware debugging

🚫 Security Notice

This system uses UID-based RFID authentication and is intended for educational purposes only.

UID-based RFID systems can be cloned and are not suitable for commercial security deployment.

🎬 Demo Video Highlights

Admin user enrollment

Paired authentication validation

Successful entry (green LED, servo actuation)

Failed entry (red LED)

Power cycle persistence demonstration

🚀 Potential Future Improvements

Lockout after multiple failed attempts

Admin delete user functionality

Event logging

Enclosure design (3D printed housing)

👨‍💻 Author

Oguzhan Besiktepe
