#include "fallguard/vision_engine.hpp"

#include <stdexcept>

namespace fallguard {

VisionEngine::VisionEngine(FallGuardConfig config)
    : config_(config)
{
}

SilhouetteFrame VisionEngine::extract_silhouette(const ImageFrame& frame) const
{
    if (frame.width() != config_.frame_width || frame.height() != config_.frame_height) {
        throw std::invalid_argument("frame dimensions do not match FallGuardConfig");
    }

    SilhouetteFrame silhouette(frame.width(), frame.height());
    for (std::uint16_t y = 0; y < frame.height(); ++y) {
        for (std::uint16_t x = 0; x < frame.width(); ++x) {
            const bool foreground = frame.at(x, y) >= config_.silhouette_threshold;
            silhouette.set(x, y, foreground && !config_.is_ignored(x, y));
        }
    }

    return silhouette;
}

} // namespace fallguard
