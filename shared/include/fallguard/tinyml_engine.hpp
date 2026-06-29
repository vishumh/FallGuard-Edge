#pragma once

#include "fallguard/config.hpp"
#include "fallguard/frame.hpp"

#include <cstdint>

namespace fallguard {

enum class PoseClass {
    Empty,
    Upright,
    Lying,
    Unknown,
};

struct BoundingBox {
    std::uint16_t x{0};
    std::uint16_t y{0};
    std::uint16_t width{0};
    std::uint16_t height{0};
};

struct ClassificationResult {
    PoseClass pose{PoseClass::Unknown};
    float confidence{0.0F};
    std::uint16_t foreground_pixels{0};
    BoundingBox bounds{};
};

class TinyMlEngine {
public:
    explicit TinyMlEngine(FallGuardConfig config);

    [[nodiscard]] ClassificationResult classify(const SilhouetteFrame& silhouette) const;

private:
    FallGuardConfig config_{};
};

const char* to_string(PoseClass pose);

} // namespace fallguard
