#pragma once

#include "fallguard/config.hpp"
#include "fallguard/frame.hpp"
#include "fallguard/image/bounding_box.hpp"

#include <cstdint>

namespace fallguard {

struct ProcessedSilhouette {
    // Silhouette containing only the pixels of the chosen subject blob.
    // Same dimensions as the input, all zero (background) if has_subject
    // is false.
    SilhouetteFrame silhouette{};
    BoundingBox bounds{};
    std::uint32_t pixel_count{0};
    bool has_subject{false};
};

// Turns a raw, per-pixel thresholded SilhouetteFrame (as produced by
// VisionEngine) into a denoised silhouette containing only the single
// largest connected blob -- i.e. "the one thing in the room big enough to
// plausibly be a person," with sensor noise and small stray objects removed.
//
// This is deliberately a *separate* stage from VisionEngine: VisionEngine's
// per-pixel threshold/ignore-zone output is still directly testable and
// usable on its own, and ImageProcessor's output type (SilhouetteFrame) is
// exactly what TinyMlEngine::classify already accepts, so it drops into the
// existing pipeline with no interface changes.
class ImageProcessor {
public:
    explicit ImageProcessor(FallGuardConfig config);

    [[nodiscard]] ProcessedSilhouette process(const SilhouetteFrame& raw_silhouette) const;

private:
    FallGuardConfig config_{};
};

} // namespace fallguard
