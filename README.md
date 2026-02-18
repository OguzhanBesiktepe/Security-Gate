🛡️ Embedded Access Control System
RFID + PIN Authentication with EEPROM Enrollment (Arduino Mega)
🧩 Problem

Unauthorized individuals are gaining access to restricted areas due to weak or single-factor authentication methods (e.g., shared PINs or physical keys).

There is a need for a low-cost embedded system that:

Restricts access to authorized users only

Requires multi-factor authentication

Supports secure user enrollment

Persists credentials after power loss

💡 Solution

A multi-factor embedded security system requiring:

4-digit PIN (Keypad)

Paired RFID Card/Tag (UID)

Both credentials must match a stored record in EEPROM for access to be granted.

An Admin Mode enables secure user enrollment and credential management.

🔐 Core Features
Multi-Factor Authentication

4-digit PIN entry via keypad

RFID scan required after PIN submission

Paired authentication (UID ↔ PIN must match)

Supports 4-byte and 7-byte UIDs

Admin Mode (EEPROM Enrollment)

Enter Admin PIN (9999)

Register new users:

Enter 4-digit PIN

Scan RFID card

Credentials saved to EEPROM

Supports up to 50 users

Persistent storage after power cycle

Embedded UI

LCD1602 (16x2 Parallel Display)

Real-time prompts:

“Enter PIN”

“Scan RFID”

“Access Granted”

“Access Denied”

Keypress audio feedback

Success and failure tones

Physical Feedback

Servo motor simulates door/gate

Green LED → Access granted

Red LED → Access denied

Passive buzzer → Audio alerts

🧠 System Architecture
State Machine Design

Authentication Flow

WAIT_PIN
   → WAIT_RFID
       → SUCCESS / FAILURE


Admin Enrollment Flow

WAIT_PIN (Admin PIN)
   → ADMIN_ENTER_PIN
       → ADMIN_SCAN_UID
           → EEPROM Write


Designed for:

Clean state transitions

Deterministic behavior

Expandability

💾 EEPROM Structure

Each user record stores:

Active flag

UID length

UID bytes (up to 7 bytes)

4-digit PIN

Includes a magic header to prevent accidental EEPROM overwrite.

🧰 Hardware Used

Arduino Mega 2560

MFRC522 RFID Reader (13.56 MHz)

RFID Cards / Key Fobs

4x4 Matrix Keypad

LCD1602 (Parallel)

10k Potentiometer (contrast)

Servo Motor

Passive Buzzer

Red & Green LEDs

220Ω Resistors

Breadboard + Jumpers

🔌 Interfaces Utilized

SPI (RFID)

Parallel 4-bit LCD

Matrix keypad scanning

PWM (Servo)

EEPROM persistent storage

📚 Engineering Concepts Demonstrated

Embedded firmware development

Multi-peripheral integration

EEPROM data management

State machine architecture

Hardware debugging

Multi-factor authentication logic

🚫 Security Notice

This project uses UID-based RFID authentication and is intended for educational purposes only.

Not suitable for commercial security deployment.

🎬 Demo Highlights

Admin user enrollment

Paired authentication validation

Successful entry (servo + LED + tone)

Failed authentication case

Power-cycle persistence demonstration

🚀 Future Improvements

Failed-attempt lockout

User deletion mode

Event logging

RTC integration

Secure RFID (DESFire EV2)

3D printed enclosure

👨‍💻 Author

Oguzhan Besiktepe
Embedded Systems & Software Engineering
