# FallGuard Edge Roadmap

## Project Vision

Develop a privacy-preserving Edge AI fall detection system capable of detecting prolonged falls using an ESP32-CAM while performing all processing locally.

---

# Current Version

v0.2.0-alpha

---

# Development Philosophy

Every sprint must leave the repository in a buildable and testable state.

Each completed sprint satisfies:

- Code implemented
- Documentation updated
- Unit tests passing
- Build passing
- Code reviewed
- GitHub issue updated

---

# Sprint Board

| Sprint | Description | Status |
|---------|-------------|--------|
| Sprint 1 | Repository Foundation | ✅ Complete |
| Sprint 2.1 | Documentation Foundation | ✅ Complete |
| Sprint 2.2 | Image Processing Pipeline | ✅ Complete |
| Sprint 2.3 | Simulator Pipeline Visualization | ⬜ Planned |
| Sprint 2.4 | Ignore Zones | ⬜ Planned |
| Sprint 2.5 | Dataset Export | ⬜ Planned |
| Sprint 2.6 | TinyML Integration | ⬜ Planned |
| Sprint 3 | ESP32 Camera Integration | ⬜ Planned |
| Sprint 4 | Web Configuration Interface | ⬜ Planned |
| Sprint 5 | Performance Optimization | ⬜ Planned |
| Sprint 6 | Release Candidate | ⬜ Planned |

---

# Long-Term Goals

- ESP32-CAM deployment
- NoIR night vision support
- TinyML posture classifier
- MQTT notifications
- OTA firmware updates
- Home Assistant integration
- Multiple camera support
- Performance benchmarking

---

# Current Sprint Focus

Sprint 2.3

Objective:

Visualize every stage of the image processing pipeline within the desktop simulator.

Expected outputs:

- Thresholded image
- Morphological output
- Largest connected component
- Bounding box
- Processing statistics