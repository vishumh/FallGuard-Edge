# ADR-0003

## Title

Image Processing Pipeline Architecture

---

## Status

Accepted

---

## Context

Initially, image processing operations were candidates for implementation directly inside VisionEngine.

As additional functionality was introduced (morphological filtering, connected components, bounding boxes), VisionEngine risked becoming responsible for multiple unrelated tasks.

This would violate the Single Responsibility Principle and make future maintenance difficult.

---

## Decision

Introduce a dedicated ImageProcessor module.

VisionEngine is responsible only for orchestrating the processing pipeline.

ImageProcessor owns all image-processing algorithms.

Future algorithms such as:

- Adaptive Thresholding
- Morphology
- Connected Components
- Blob Tracking
- Skeletonization
- Dataset Export

will be implemented inside ImageProcessor.

---

## Architecture

Camera

↓

Vision Engine

↓

Image Processor

↓

Morphology

↓

Connected Components

↓

Bounding Box

↓

TinyML

↓

Decision Engine

---

## Consequences

Advantages

- Clear separation of responsibilities
- Easier testing
- Easier maintenance
- Portable implementation
- Platform independent shared library
- Cleaner VisionEngine

Trade-offs

- Additional abstraction layer
- Slight increase in project complexity

The architectural benefits significantly outweigh the additional complexity.

---

## Future Considerations

Future image-processing algorithms should be implemented as independent processing stages rather than expanding VisionEngine.

This architecture is expected to scale to additional edge vision applications beyond fall detection.