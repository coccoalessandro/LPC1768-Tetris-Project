# Tetris Embedded - ARM LPC1768 Project
### *Politecnico di Torino - Computer Architectures (Extra Points Project)*

![Language](https://img.shields.io/badge/Language-C%20%26%20ARM%20Assembly-blue)
![Platform](https://img.shields.io/badge/Platform-LandTiger%20Board-orange)
![Microcontroller](https://img.shields.io/badge/MCU-LPC1768%20(Cortex--M3)-red)

## 🕹️ Project Overview
This repository contains a full-featured implementation of **Tetris** developed for the **LandTiger Board (NXP LPC1768)**. The project was developed as an advanced exercise for the *Computer Architectures* course, focusing on low-level system programming, hardware-software co-design, and real-time resource management.

The game challenges the player to stack "tetrominoes" to clear lines and achieve high scores, featuring increasing difficulty, power-ups, and sound effects.

## 🛠️ Key Technical Features
- **Hybrid Programming:** Core logic developed in **C** for flexibility, with critical sections and peripheral management optimized in **ARM Assembly**.
- **Interrupt-Driven Logic:**
  - **Timer 0 (50ms):** Handles the game tick, falling speed, and real-time gravity.
  - **Timer 1 (DAC):** Manages background music and sound effects (Power-ups, Line Clear).
  - **EINT0 (KEY1/KEY2):** Used for game start, pause, and hard-reset functionalities.
- **Peripheral Management:**
  - **LCD Display (ST7735):** Real-time rendering of the 10x22 game field, ghost pieces, and UI (Score, High Score, Next Piece).
  - **Joystick Control:** Implementation of movement (Left/Right), rotation, and "soft drop" via polling/interrupts.
  - **Speaker (DAC):** Audio feedback for game events and background music.

## 🚀 Game Mechanics & Specifications
- **Tetrominoes:** All 7 standard shapes (I, J, L, O, S, T, Z) with accurate rotation matrices.
- **Scoring System:** - Single line: 100 pts | Tetris (4 lines): 600 pts.
  - High Score persistence during the session.
- **Advanced Power-ups:**
  - 💥 **Half-clear:** Instantly clears the bottom half of the field.
  - 🐢 **Slow-down:** Reduces game speed to 1 square/sec for 15 seconds.
  - ⚠️ **Random Malus:** Every 10 lines, a "garbage line" with 7 random blocks is added from the bottom.

## 📁 Repository Structure
```text
├── src/
│   ├── main.c              # Game loop and initialization
│   ├── timer/              # Timer 0/1 ISR and configuration
│   ├── joystick/           # Input handling logic
│   ├── lcd/                # Graphic drivers and drawing functions
│   ├── music/              # RTTTL player and DAC management
│   └── asm/                # ARM Assembly optimized routines
├── project.uvprojx         # Keil uVision project file
└── docs/                   # Setup and technical specifications
