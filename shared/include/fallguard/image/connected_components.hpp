#pragma once

#include "fallguard/frame.hpp"
#include "fallguard/image/bounding_box.hpp"

#include <cstdint>
#include <vector>

namespace fallguard {

enum class Connectivity {
    Four,
    Eight,
};

struct ConnectedComponent {
    BoundingBox bounds{};
    std::uint32_t pixel_count{0};
};

// Labels foreground blobs in a SilhouetteFrame using an iterative flood
// fill (no recursion, so stack usage stays flat regardless of blob size --
// important on the ESP32's limited stack).
//
// If labels_out is non-null, it is resized to width*height and filled with
// the component index (0-based, matching the returned vector) for every
// foreground pixel and -1 for background pixels. This lets a caller isolate
// the pixels belonging to a single chosen component.
[[nodiscard]] std::vector<ConnectedComponent> find_connected_components(
    const SilhouetteFrame& silhouette,
    Connectivity connectivity = Connectivity::Eight,
    std::vector<std::int32_t>* labels_out = nullptr);

// Returns a pointer to the largest-by-pixel-count component, or nullptr if
// components is empty. The pointer aliases into the input vector.
[[nodiscard]] const ConnectedComponent* largest_component(
    const std::vector<ConnectedComponent>& components);

} // namespace fallguard
