# FallGuard Edge

Privacy-preserving TinyML fall detection using ESP32-CAM.

## Overview

FallGuard Edge is an open-source Edge AI project that detects when a person has fallen and remains lying on the floor for a configurable duration using an ESP32-CAM.

Unlike conventional CCTV systems, all image processing is intended to run locally on the device. No cloud services are required for detection, which protects privacy and keeps deployment cost low.

The project combines classical computer vision techniques with TinyML posture classification to create a low-cost fall detection system for homes, assisted living facilities, clinics, and research.

## Current Status

Version: `v0.1.0-alpha`

Sprint: `Sprint 1 - Foundation`

Implemented foundation and early Sprint 2 pieces:

- Shared C++17 module structure
- Camera frame-source abstraction
- Config and ignore-zone model
- Grayscale frame and binary silhouette frame types
- Threshold-based silhouette extraction
- Heuristic posture classifier placeholder for TinyML integration
- Fall decision engine with configurable confirmation timer
- Alert manager
- Desktop simulator with synthetic frames or PGM image-sequence input
- CTest-based foundation tests
- PlatformIO ESP32-CAM skeleton

The current classifier is intentionally a lightweight placeholder. It uses silhouette bounding-box aspect ratio to separate upright and lying shapes so the rest of the pipeline can be developed and tested before a quantized TinyML model is added.

## Key Features

- Privacy-first architecture
- 100% on-device processing target
- TinyML posture classification target
- Configurable fall confirmation timer
- Ignore zones for beds, sofas, and other safe lying areas
- Binary silhouette processing
- Automatic day/night operation target
- Local Web UI target
- Local alarm support target
- ESP32-CAM based hardware target
- Desktop simulator for development
- Modular C++ architecture

## Privacy First

By design, FallGuard Edge should:

- Never upload images to the cloud
- Never record video by default
- Never perform facial recognition
- Never identify individuals
- Process frames entirely in device memory

The target TinyML model operates on binary silhouettes rather than raw images whenever possible.

## System Architecture

```text
Camera
  |
  v
Image Processing
  |
  v
Binary Silhouette Extraction
  |
  v
TinyML Classification
  |
  v
Decision Engine
  |
  v
Alert Manager
```

## Repository Structure

```text
firmware/esp32/       ESP32-CAM PlatformIO firmware skeleton
shared/               Shared C++17 detection pipeline
simulator/            Desktop simulator executable
tests/                CTest foundation tests
training/             Planned dataset and model training tools
datasets/             Planned local datasets, ignored by default when added
docs/                 Planned design and deployment documentation
hardware/             Planned wiring and enclosure notes
tools/                Planned development utilities
```

## Build: Desktop Simulator

```bash
cmake -S . -B build
cmake --build build
./build/simulator/fallguard_simulator
```

Expected simulator behavior:

- The first synthetic frames contain an upright silhouette.
- Later frames contain a lying silhouette.
- The state moves from `Monitoring` to `SuspectedFall`.
- After the configured confirmation timer, the state becomes `FallConfirmed` and the alert turns on.

The simulator can also process grayscale PGM image sequences:

```bash
./build/simulator/fallguard_simulator \
  tests/fixtures/upright_16.pgm \
  tests/fixtures/lying_16.pgm \
  tests/fixtures/lying_16.pgm \
  tests/fixtures/lying_16.pgm \
  tests/fixtures/lying_16.pgm
```

Supported image formats are 8-bit `P2` and `P5` PGM files. Every image in a sequence must have the same dimensions. This gives the vision pipeline a simple desktop input path before ESP32-CAM capture is wired in.

## Run Tests

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Build: ESP32 Firmware

Install PlatformIO, then run:

```bash
cd firmware/esp32
pio run
```

The firmware currently boots the foundation skeleton and links against the shared project library. Camera capture, ESP32-CAM pin configuration, image preprocessing, and TinyML model execution are planned for later sprints.

## Planned Hardware

Version 1 target hardware:

- ESP32-CAM AI Thinker
- OV2640 NoIR Camera
- 4 MB PSRAM
- IR LED array
- Buzzer
- Optional ambient light sensor
- Optional Wi-Fi connectivity

## Development Roadmap

The project should remain compilable at the end of every sprint.

### Sprint 1 - Foundation

- Shared module layout
- Simulator build
- Firmware skeleton
- Core config, logging, frame, decision, and alert modules
- Basic tests

### Sprint 2 - Vision Engine

- Camera frame-source abstraction
- Simulator PGM image-sequence input
- ESP32 camera capture abstraction
- Background subtraction or adaptive thresholding
- Noise filtering and morphology
- Binary silhouette visualization
- Ignore-zone editing and persistence

### Sprint 3 - TinyML Integration

- Dataset builder for upright, lying, empty, and ambiguous silhouettes
- Model training pipeline
- Quantization
- TensorFlow Lite Micro integration
- Model confidence thresholds

### Sprint 4 - Detection Engine

- Temporal smoothing
- False-positive suppression
- Fall confirmation and recovery rules
- Alert escalation policy

### Sprint 5 - Web Interface

- Local configuration page
- Ignore-zone editor
- Silhouette preview
- Alarm status and reset control

### Sprint 6 - ESP32 Deployment

- ESP32-CAM pin map and camera initialization
- PSRAM-aware frame buffers
- Buzzer/relay output
- Day/night IR support
- Wi-Fi local-only mode

### Sprint 7 - Testing and Optimization

- Performance benchmarks
- Memory profiling
- Long-running stability tests
- Field test checklist
- CI builds for simulator tests

## Design Principles

- Modern C++17
- Modular architecture
- Hardware abstraction layer
- Shared desktop and firmware code
- Test-driven development where practical
- Minimal dynamic memory allocation on firmware paths
- Production-quality documentation
- Open-source friendly

## Future Goals

- MQTT support
- Home Assistant integration
- OTA firmware updates
- Multiple camera support
- Improved TinyML models
- Performance benchmarking
- Automated testing
- Continuous integration

## Contributing

Contributions, bug reports, and feature suggestions are welcome. Please read `CONTRIBUTING.md` before submitting pull requests.

## License

This project is licensed under the MIT License.
