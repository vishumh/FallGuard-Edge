#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace fallguard {

class ImageFrame {
public:
    ImageFrame() = default;
    ImageFrame(std::uint16_t width, std::uint16_t height, std::uint8_t fill = 0);

    [[nodiscard]] std::uint16_t width() const;
    [[nodiscard]] std::uint16_t height() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::uint8_t at(std::uint16_t x, std::uint16_t y) const;
    void set(std::uint16_t x, std::uint16_t y, std::uint8_t value);

private:
    std::uint16_t width_{0};
    std::uint16_t height_{0};
    std::vector<std::uint8_t> pixels_{};
};

class SilhouetteFrame {
public:
    SilhouetteFrame() = default;
    SilhouetteFrame(std::uint16_t width, std::uint16_t height);

    [[nodiscard]] std::uint16_t width() const;
    [[nodiscard]] std::uint16_t height() const;
    [[nodiscard]] bool at(std::uint16_t x, std::uint16_t y) const;
    void set(std::uint16_t x, std::uint16_t y, bool foreground);

private:
    std::uint16_t width_{0};
    std::uint16_t height_{0};
    std::vector<std::uint8_t> pixels_{};
};

} // namespace fallguard
