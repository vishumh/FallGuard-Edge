#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

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
    // Morphological open() iterations applied before blob selection to
    // remove sensor noise. 0 disables denoising entirely.
    std::uint8_t denoise_iterations{1};
    std::array<IgnoreZone, max_ignore_zones> ignore_zones{};
    std::size_t ignore_zone_count{0};

    [[nodiscard]] bool add_ignore_zone(IgnoreZone zone);
    [[nodiscard]] bool is_ignored(std::uint16_t x, std::uint16_t y) const;
    void clear_ignore_zones();

    // Text persistence for ignore zones, e.g. "x,y,w,h;x,y,w,h". Kept as
    // plain string (de)serialization rather than file I/O so this stays
    // usable from both the desktop simulator (which owns std::fstream) and
    // firmware (which will persist the string via SPIFFS/LittleFS/NVS).
    [[nodiscard]] std::string serialize_ignore_zones() const;

    // Replaces all ignore zones with the ones parsed from `text`. Returns
    // false (leaving existing zones untouched) if the text is malformed or
    // contains more zones than max_ignore_zones.
    [[nodiscard]] bool load_ignore_zones_from_string(const std::string& text);
};

} // namespace fallguard
