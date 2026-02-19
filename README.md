# Embedded Access Control System (RFID + PIN)

An embedded multi-factor security system built on the **Arduino Mega 2560** that restricts access using a paired **4-digit PIN** and **RFID card (UID)**.

The system features **EEPROM-backed user enrollment**, LCD-based UI prompts, and physical gate actuation using a servo motor.

This project combines **hardware integration**, **persistent storage**, and **structured state-machine firmware design (C++)**.

---

## 📹 Demo Video

▶️ [Watch the Full Demo on YouTube](https://youtu.be/t_v2mCIrs5Q)

---

## 🧩 Problem

Unauthorized individuals are gaining access to restricted areas due to weak or single-factor authentication methods (e.g., shared PINs or physical keys).

There is a need for a low-cost embedded system that:
- Restricts access to **authorized users only**
- Requires **multi-factor authentication**
- Supports secure **user enrollment**
- Persists crednetials after power loss

---

## 💡 Solution

A multi-factor embedded security system requiring:

- **4-digit PIN (Keypad)**
- **Paired RFID Card/Tag (UID)**

---

## 🔐 System Overview

- User enters a **4-digit PIN** via keypad  
- User scans an **RFID card**  
- System verifies **paired credentials from EEPROM**

### If valid:
- **Green LED**
- **Success tone**
- **Servo opens gate**

### If invalid:
- **Red LED**
- **Failure tone**
- **Gate remains closed**

---

## 🛠 Admin Mode

- Enter **Admin PIN (9999)**
- Create new users:
  - Enter **4-digit PIN**
  - Scan **RFID card**
  - Credentials saved to **EEPROM**
- Supports up to **50 users**
- Credentials persist after **power cycle**

---

## 🧠 Key Features

- **Multi-factor authentication** (PIN + RFID)
- **Paired credential verification** (UID ↔ PIN)
- **Finite State Machine** for structured control flow
- **EEPROM persistent storage**
- **Parallel LCD1602 interface** (4-bit mode)
- **SPI communication** for RFID
- **Real-time hardware feedback** (LED + buzzer + servo)

---

## 🛠 Hardware Used

- Arduino Mega 2560
- MFRC522 RFID Reader (SPI)
- 4x4 Matrix Keypad
- LCD1602 (Parallel, 4-bit mode)
- Servo Motor
- Passive Piezo Buzzer
- Red & Green LEDs (with resistors)
- 10k Potentiometer (LCD contrast)
- Breadboard + jumper wires

---

## 💻 Software Details

- Written in **C++*
- Built using **PlatformIO**
- Structured as a **state-machine architecture**
- EEPROM records stored using `EEPROM.put()` and `EEPROM.get()`
- SPI communication via `MFRC522`
- LCD controlled with `LiquidCrystal`

---

## 📈 What This Project Demonstrates

- Embedded systems design
- Multi-peripheral hardware integration
- Persistent credential management
- Authentication system architecture
- State-based firmware logic
- Hardware debugging & signal validation

---

## 🚀 Future Improvements

- Failed-attempt lockout
- Admin user deletion
- Secure RFID implementation
- 3D printed enclosure
