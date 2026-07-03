#pragma once

#include <cstdint>

namespace fallguard {

// Axis-aligned bounding box in frame pixel coordinates.
struct BoundingBox {
    std::uint16_t x{0};
    std::uint16_t y{0};
    std::uint16_t width{0};
    std::uint16_t height{0};

    // Width / height, guarding against a zero-height box so callers never
    // have to special-case division by zero.
    [[nodiscard]] float aspect_ratio() const;

    [[nodiscard]] std::uint32_t area() const;
};

} // namespace fallguard
