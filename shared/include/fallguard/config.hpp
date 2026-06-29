#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace fallguard {

struct IgnoreZone {
    std::uint16_t x{0};
    std::uint16_t y{0};
    std::uint16_t width{0};
    std::uint16_t height{0};

    [[nodiscard]] bool contains(std::uint16_t px, std::uint16_t py) const;
};

struct FallGuardConfig {
    static constexpr std::size_t max_ignore_zones = 4;

    std::uint16_t frame_width{96};
    std::uint16_t frame_height{96};
    std::uint8_t silhouette_threshold{80};
    std::uint16_t min_foreground_pixels{24};
    float lying_aspect_ratio{1.55F};
    std::uint32_t fall_confirmation_ms{3000};
    std::array<IgnoreZone, max_ignore_zones> ignore_zones{};
    std::size_t ignore_zone_count{0};

    [[nodiscard]] bool add_ignore_zone(IgnoreZone zone);
    [[nodiscard]] bool is_ignored(std::uint16_t x, std::uint16_t y) const;
};

} // namespace fallguard
