# FallGuard Edge

## Purpose

FallGuard Edge is a privacy-first embedded vision framework whose first application is fall detection.

The project is intentionally designed to be reusable for future edge vision applications.

---

# Hardware Target

Primary Platform

- ESP32-CAM AI Thinker
- OV2640 NoIR
- 4 MB PSRAM

Future Platform

- ESP32-P4
- OV5647

---

# Software Stack

Desktop

- C++17
- CMake
- OpenCV
- GoogleTest

Firmware

- PlatformIO
- Arduino Framework

Training

- TensorFlow
- TensorFlow Lite
- Python

---

# Design Principles

- Platform independent shared library
- No OpenCV inside shared/
- One responsibility per class
- Modern C++17
- Test driven where practical
- Documentation evolves with implementation

---

# Current Architecture

Camera

↓

Vision Engine

↓

Image Processor

↓

Morphological Filtering

↓

Connected Component Detection

↓

Bounding Box Extraction

↓

TinyML

↓

Decision Engine

↓

Alert Manager

---

# Completed Modules

- Configuration
- Logger
- Camera abstraction
- Vision Engine
- Image Processor
- Morphology
- Connected Components
- Bounding Box extraction
- Desktop simulator foundation

---

# Upcoming Work

Sprint 2.3

- Visual debugging
- Bounding box rendering
- Performance counters

Sprint 2.4

- Ignore zone editor

Sprint 2.5

- Dataset exporter

Sprint 2.6

- TinyML inference