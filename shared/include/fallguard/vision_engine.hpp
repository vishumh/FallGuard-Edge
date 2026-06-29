#pragma once

#include "fallguard/config.hpp"
#include "fallguard/frame.hpp"

namespace fallguard {

class VisionEngine {
public:
    explicit VisionEngine(FallGuardConfig config);

    [[nodiscard]] SilhouetteFrame extract_silhouette(const ImageFrame& frame) const;

private:
    FallGuardConfig config_{};
};

} // namespace fallguard
