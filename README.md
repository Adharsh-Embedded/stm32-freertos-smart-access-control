# 🚀 STM32 FreeRTOS Smart Access Control & Attendance System

A real-time **RFID-based Smart Access Control & Attendance System** developed using **STM32 Nucleo-F446RE** and **FreeRTOS**.

This project demonstrates how a real embedded system can authenticate RFID cards, record attendance with timestamps, provide user feedback through an LCD, generate alarms for unauthorized access, and log system events using multiple FreeRTOS tasks.

---

## 📌 Features

- RFID-based employee authentication
- Supports multiple authorized RFID cards
- Detects unauthorized cards
- Attendance logging with Date & Time
- Prevents duplicate attendance marking
- LCD display for user interaction
- UART logging for monitoring
- LED and buzzer indication
- Periodic system health monitoring using FreeRTOS Software Timer

---

## 🛠 Hardware Used

- STM32 Nucleo-F446RE
- RC522 RFID Reader
- 16x2 I2C LCD
- Buzzer
- LEDs
- RFID Cards / Tags

---

## 💻 Software Used

- STM32CubeIDE
- STM32 HAL Library
- FreeRTOS (CMSIS-RTOS v2)

---

## ⚡ FreeRTOS Architecture

### RFID Task
- Detects RFID cards
- Reads card UID
- Identifies authorized employees
- Sends information to other tasks

### LCD Task
- Displays:
  - Tap Card
  - Welcome Screen
  - Employee Name
  - Date & Time
  - Access Denied
  - Attendance Already Marked

### UART Task
- Logs attendance details
- Prints employee information
- Displays date and time
- Prints system events

### Alarm Task
- Controls LED and Buzzer
- Generates alarm for unauthorized access
- Gives visual indication for successful authentication

### Software Timer
- Periodically prints:
  - System Running
  - Number of attendance records

---

## 🔄 FreeRTOS Objects Used

### Tasks
- RFID Task
- LCD Task
- UART Task
- Alarm Task

### Queue
Transfers attendance information from RFID Task to UART Task.

### Task Notification
Notifies LCD Task whenever a card is detected.

### Binary Semaphore
Triggers the Alarm Task after card processing.

### Mutex
Protects shared resources such as LCD and UART during concurrent access.

### Software Timer
Used for periodic system status updates.

---

## 📊 System Workflow

```text
           RFID Card
               │
               ▼
        RFID Detection Task
               │
        Check Card UID
        ┌───────────────┐
        │               │
   Authorized      Unauthorized
        │               │
        ▼               ▼
 Record Attendance   Access Denied
        │               │
        ├──────┬────────┤
        ▼      ▼        ▼
     LCD     UART    Alarm Task
        │
        ▼
   Wait for Next Card
```

---

## 📺 LCD Display

```
Tap Your
Card!
```

```
Welcome
Employee 1
```

```
Time : 09:30:15
Date : 27/06/26
```

```
Attendance
Already Marked
```

```
Access
Denied!
```

---

## 🖥 UART Output

```
Employee 1

Time : 09:30:15
Date : 27/06/26
```

```
Access Denied!
```

```
System Running
Cards Registered : 2
```

---

## 🔧 STM32 Peripherals Used

- SPI
- I2C
- UART
- RTC
- GPIO

---

## 📚 Concepts Demonstrated

- Embedded C Programming
- STM32 HAL
- FreeRTOS Multitasking
- Inter-task Communication
- Task Synchronization
- Resource Protection
- RFID Communication
- Real-Time Clock Integration
- LCD Interfacing
- UART Debugging

---

## 🎯 Applications

- Employee Attendance System
- Smart Door Lock
- Office Access Control
- Laboratory Access System
- College Attendance System

---

## 🚀 Future Improvements

- EEPROM Storage
- SD Card Logging
- Fingerprint Authentication
- Password + RFID Authentication
- IoT Dashboard
- Wi-Fi Attendance Upload
- Cloud Database Integration
- Mobile Application Support

---

## 👨‍💻 Author

**Adharsh**

Embedded Systems | STM32 | FreeRTOS | Embedded C | IoT

---

⭐ If you found this project useful, don't forget to star the repository!
