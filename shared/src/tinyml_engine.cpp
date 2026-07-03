#include "fallguard/tinyml_engine.hpp"

#include <algorithm>
#include <limits>

namespace fallguard {

TinyMlEngine::TinyMlEngine(FallGuardConfig config)
    : config_(config)
{
}

ClassificationResult TinyMlEngine::classify(const SilhouetteFrame& silhouette) const
{
    ClassificationResult result{};
    auto min_x = std::numeric_limits<std::uint16_t>::max();
    auto min_y = std::numeric_limits<std::uint16_t>::max();
    std::uint16_t max_x = 0;
    std::uint16_t max_y = 0;

    for (std::uint16_t y = 0; y < silhouette.height(); ++y) {
        for (std::uint16_t x = 0; x < silhouette.width(); ++x) {
            if (!silhouette.at(x, y)) {
                continue;
            }

            ++result.foreground_pixels;
            min_x = std::min(min_x, x);
            min_y = std::min(min_y, y);
            max_x = std::max(max_x, x);
            max_y = std::max(max_y, y);
        }
    }

    if (result.foreground_pixels < config_.min_foreground_pixels) {
        result.pose = PoseClass::Empty;
        result.confidence = 1.0F;
        return result;
    }

    result.bounds.x = min_x;
    result.bounds.y = min_y;
    result.bounds.width = static_cast<std::uint16_t>(max_x - min_x + 1);
    result.bounds.height = static_cast<std::uint16_t>(max_y - min_y + 1);

    const auto aspect_ratio = result.bounds.aspect_ratio();

    if (aspect_ratio >= config_.lying_aspect_ratio) {
        result.pose = PoseClass::Lying;
        result.confidence = std::min(0.99F, aspect_ratio / (config_.lying_aspect_ratio * 2.0F));
    } else {
        result.pose = PoseClass::Upright;
        result.confidence = std::min(0.99F, 1.0F / std::max(0.1F, aspect_ratio));
    }

    return result;
}

const char* to_string(PoseClass pose)
{
    switch (pose) {
    case PoseClass::Empty:
        return "Empty";
    case PoseClass::Upright:
        return "Upright";
    case PoseClass::Lying:
        return "Lying";
    case PoseClass::Unknown:
        return "Unknown";
    }

    return "Unknown";
}

} // namespace fallguard
