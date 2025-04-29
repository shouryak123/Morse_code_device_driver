# Morse Code Decoder using Linux Device Driver and ESP32

This project implements a Linux character device driver that decodes Morse code transmitted from an ESP32 microcontroller via serial (UART) communication.

## 📌 Overview

- An **ESP32** sends Morse code input (dots `.` and dashes `-`) over serial.
- A **Linux kernel module** (`morse_driver`) receives and decodes this input.
- A user-space C program reads serial input from the ESP32 and passes it to the kernel via `ioctl`.
- The kernel decodes the Morse code and writes the decoded message to a text file.

---

## 🧩 Features

- Custom Linux **character device driver** (`/dev/morse_driver`).
- Uses **`ioctl` commands** to send Morse input (`MORSE_IOC_SEND`) and reset (`MORSE_IOC_RESET`).
- **Kernel timer** and **workqueue** for asynchronous decoding.
- Thread-safe buffer handling using `mutex`.
- Morse decoding handled in kernel space.
- Output is appended to a file (`morse_driver.txt`).



