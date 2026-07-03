# FallGuard Edge Engineering Notebook

---

# Sprint 2.1

## Objective

Establish project documentation and repository workflow.

## Completed

- Documentation framework
- ADR structure
- Sprint planning
- Engineering process

## Result

PASS

---

# Sprint 2.2

## Objective

Implement a modular image processing pipeline.

## Implemented

- ImageProcessor abstraction
- Morphological filtering
- Connected component detection
- Largest blob extraction
- Bounding box extraction

## Why

Preparing the project for TinyML inference required separating image processing from the Vision Engine.

Rather than extending VisionEngine indefinitely, a dedicated ImageProcessor module was introduced.

## Lessons Learned

- Keep VisionEngine small.
- Algorithms belong inside ImageProcessor.
- Morphology significantly reduces threshold noise.
- Largest connected component provides a reliable person candidate.

## Technical Debt

None identified.

## Performance Notes

Current implementation is suitable for ESP32-CAM target frame rates (1–2 FPS).

## Repository Health

Build

PASS

Tests

PASS

Documentation

Updated

Issue

#1 (Ready for Functional Validation)

---

# Next Sprint

Sprint 2.3

Simulator visualization.

Objective:

Display every image processing stage for debugging.