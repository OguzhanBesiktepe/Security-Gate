🔐 Embedded Access Control System
RFID + PIN Multi-Factor Authentication (Arduino Mega 2560)

An embedded multi-factor authentication system built on the Arduino Mega 2560 that restricts access using a paired 4-digit PIN and RFID UID.

This project integrates:

Persistent credential storage (EEPROM)

Structured state-machine firmware (C++)

Multi-peripheral hardware control

Physical gate actuation via servo motor

Real-time user feedback via LCD, LEDs, and buzzer

📹 Demo Video

🎥 (Insert YouTube demo link here)

🧩 System Architecture Overview
Authentication Flow

User enters a 4-digit PIN via keypad

User scans an RFID card

System verifies paired credentials stored in EEPROM

If Credentials Are Valid

🟢 Green LED activates

🔔 Success tone plays

🔓 Servo motor opens gate

If Credentials Are Invalid

🔴 Red LED activates

🔕 Failure tone plays

🔒 Gate remains closed

🛠 Admin Mode
Enter Admin PIN: 9999

Admin capabilities:

Create new users

Enter new 4-digit PIN

Scan RFID card

Credentials stored in EEPROM

Supports up to 50 users

Credentials persist after power cycle

🧠 Key Technical Features

Multi-factor authentication (PIN + RFID)

Paired credential verification (UID ↔ PIN)

Finite State Machine firmware architecture

EEPROM-based persistent storage

Parallel LCD1602 (4-bit mode)

SPI communication (RFID)

Real-time hardware feedback system

🛠️ Hardware Used

Arduino Mega 2560

MFRC522 RFID Reader (SPI)

4x4 Matrix Keypad

LCD1602 (Parallel, 4-bit mode)

Servo Motor

Passive Piezo Buzzer

Red & Green LEDs (with resistors)

10k Potentiometer (LCD contrast control)

Breadboard + jumper wires

💻 Software Architecture

Language: C++ (Arduino framework)

Built using PlatformIO

Structured as a Finite State Machine

EEPROM storage using:

EEPROM.put()

EEPROM.get()

SPI handled via MFRC522 library

LCD controlled using LiquidCrystal

🧱 Firmware Design (State Machine)

The firmware is structured into controlled states such as:

Idle

PIN Entry

RFID Scan

Verification

Access Granted

Access Denied

Admin Enrollment

This state-based architecture improves:

Readability

Debugging efficiency

Scalability

Maintainability

📈 What This Project Demonstrates

Embedded systems design principles

Multi-peripheral hardware integration

Persistent credential management

Authentication system implementation

Structured firmware architecture

Hardware debugging & validation workflow

This project reflects a transition from hobby-level Arduino projects to structured embedded system development.

🚀 Future Improvements

Failed-attempt lockout protection

Admin user deletion functionality

Access event logging

Real-Time Clock (RTC) integration

Secure RFID implementation (e.g., DESFire)

Custom PCB design

3D-printed enclosure
