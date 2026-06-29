#pragma once

#include "fallguard/frame.hpp"

#include <cstdint>

namespace fallguard {

struct CapturedFrame {
    ImageFrame image{};
    std::uint32_t timestamp_ms{0};
};

class CameraFrameSource {
public:
    virtual ~CameraFrameSource() = default;

    [[nodiscard]] virtual bool read(CapturedFrame& frame) = 0;
    [[nodiscard]] virtual std::uint16_t width() const = 0;
    [[nodiscard]] virtual std::uint16_t height() const = 0;
};

} // namespace fallguard
