<div align="center">

# 🎮 Multi-Matrix

**Arduino-based multi-purpose gadget & retro game console**  
*8×8 LED matrix • Gyroscope • Magnetometer • Touch controls*

![Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Modes](https://img.shields.io/badge/Modes-8-blueviolet?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Working-brightgreen?style=for-the-badge)

</div>

---

## 📋 Overview

Multi-Matrix turns a single Arduino, an 8×8 LED matrix, and two I²C sensors into **8 different apps and games** — all switchable on the fly with 4 touch buttons and a shift key.

---

## ✨ Modes

| # | Icon | Mode | How it works |
|:-:|:----:|------|-------------|
| 0 | 🧭 | **Needle Compass** | Magnetometer → single LED points North |
| 1 | 🔤 | **Letter Compass** | Shows N / S / E / W as full letters |
| 2 | 🎳 | **Arcade Bowling** | Tilt to aim, shake to roll |
| 3 | 🐍 | **Snake** | Tilt to steer, eat food, don't crash |
| 4 | 🐦 | **Flappy Dot** | SHIFT to flap, dodge the pipes |
| 5 | 🎲 | **Dice** | Shake to roll — result on matrix |
| 6 | 🎱 | **Magic 8-Ball** | Shake for Yes / No / Maybe |
| 7 | 🔵 | **Bubble Level** | Tilt the board, center the dot |

---

## 🕹️ Controls

| Button | Normal Press | + SHIFT Held |
|--------|-------------|--------------|
| **TOUCH 1** | Needle Compass | Letter Compass |
| **TOUCH 2** | Bowling | Snake |
| **TOUCH 3** | Flappy Dot | Dice |
| **TOUCH 4** | Magic 8-Ball | Bubble Level |
| **SHIFT** | *(modifier)* | Flappy Bird jump |

---

## 🔧 Hardware Required

| Component | Specification |
|-----------|--------------|
| Microcontroller | Arduino Uno / Mega |
| Display | 8×8 MAX7219 LED Matrix `FC16` |
| IMU | MPU-6050 · I²C @ `0x68` |
| Compass | HMC5883L · I²C @ `0x1E` |
| Input | 4× Touch / Tactile buttons |
| Audio | Passive buzzer |

---

## 📌 Pin Map

| Pin | Function |
|-----|----------|
| 10 | Matrix CS (SPI) |
| 2 | TOUCH 1 |
| 3 | TOUCH 2 |
| 4 | TOUCH 3 |
| 5 | TOUCH 4 |
| 6 | SHIFT |
| 7 | Buzzer |
| SDA/SCL | MPU-6050 + HMC5883L |

---

## 📦 Dependencies

Install via **Arduino Library Manager**:

- [`MD_MAX72XX`](https://github.com/MajicDesigns/MD_MAX72XX) — MajicDesigns
- `Wire.h` — built-in

---

## ⚙️ Calibration

At the top of the sketch, set your IMU offsets:

```cpp
int16_t offsetX = 0;  // ← replace with your X offset
int16_t offsetY = 0;  // ← replace with your Y offset
```

> **Tip:** Lay the device flat, open Serial Monitor at **9600 baud**, and read the raw `calX` / `calY` values — those are your offsets.

---

## 🚀 Quickstart

1. Wire components per the diagram above
2. Install MD_MAX72XX via Library Manager
3. Set calibration offsets in the sketch
4. Upload → enjoy!

---

## 📄 License

MIT — see [`LICENSE`](LICENSE)
