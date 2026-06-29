#include "fallguard/config.hpp"

namespace fallguard {

bool IgnoreZone::contains(std::uint16_t px, std::uint16_t py) const
{
    const auto right = static_cast<std::uint32_t>(x) + width;
    const auto bottom = static_cast<std::uint32_t>(y) + height;
    return px >= x && py >= y && px < right && py < bottom;
}

bool FallGuardConfig::add_ignore_zone(IgnoreZone zone)
{
    if (ignore_zone_count >= ignore_zones.size()) {
        return false;
    }

    ignore_zones[ignore_zone_count] = zone;
    ++ignore_zone_count;
    return true;
}

bool FallGuardConfig::is_ignored(std::uint16_t x, std::uint16_t y) const
{
    for (std::size_t i = 0; i < ignore_zone_count; ++i) {
        if (ignore_zones[i].contains(x, y)) {
            return true;
        }
    }

    return false;
}

} // namespace fallguard
