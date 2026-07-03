#include "fallguard/config.hpp"

#include <charconv>
#include <sstream>
#include <string>
#include <vector>

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

void FallGuardConfig::clear_ignore_zones()
{
    ignore_zone_count = 0;
}

std::string FallGuardConfig::serialize_ignore_zones() const
{
    std::ostringstream out;
    for (std::size_t i = 0; i < ignore_zone_count; ++i) {
        if (i > 0) {
            out << ';';
        }
        const auto& zone = ignore_zones[i];
        out << zone.x << ',' << zone.y << ',' << zone.width << ',' << zone.height;
    }
    return out.str();
}

namespace {

bool parse_uint16(const std::string& token, std::uint16_t& value_out)
{
    unsigned long parsed = 0;
    const auto* begin = token.data();
    const auto* end = token.data() + token.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc{} || result.ptr != end || parsed > 0xFFFFU) {
        return false;
    }
    value_out = static_cast<std::uint16_t>(parsed);
    return true;
}

bool parse_zone(const std::string& token, IgnoreZone& zone_out)
{
    std::array<std::uint16_t, 4> fields{};
    std::size_t field_index = 0;
    std::size_t start = 0;

    while (start <= token.size()) {
        const auto comma = token.find(',', start);
        const auto end = comma == std::string::npos ? token.size() : comma;
        if (field_index >= fields.size()) {
            return false;
        }
        if (!parse_uint16(token.substr(start, end - start), fields[field_index])) {
            return false;
        }
        ++field_index;
        if (comma == std::string::npos) {
            break;
        }
        start = comma + 1;
    }

    if (field_index != fields.size()) {
        return false;
    }

    zone_out = IgnoreZone{fields[0], fields[1], fields[2], fields[3]};
    return true;
}

} // namespace

bool FallGuardConfig::load_ignore_zones_from_string(const std::string& text)
{
    if (text.empty()) {
        clear_ignore_zones();
        return true;
    }

    std::vector<IgnoreZone> parsed_zones;
    std::size_t start = 0;

    while (start <= text.size()) {
        const auto semicolon = text.find(';', start);
        const auto end = semicolon == std::string::npos ? text.size() : semicolon;
        const auto token = text.substr(start, end - start);

        if (!token.empty()) {
            IgnoreZone zone{};
            if (!parse_zone(token, zone)) {
                return false;
            }
            if (parsed_zones.size() >= max_ignore_zones) {
                return false;
            }
            parsed_zones.push_back(zone);
        }

        if (semicolon == std::string::npos) {
            break;
        }
        start = semicolon + 1;
    }

    clear_ignore_zones();
    for (const auto& zone : parsed_zones) {
        if (!add_ignore_zone(zone)) {
            return false;
        }
    }
    return true;
}

} // namespace fallguard
