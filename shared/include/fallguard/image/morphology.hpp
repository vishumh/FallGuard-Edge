#pragma once

#include "fallguard/frame.hpp"

#include <cstdint>

namespace fallguard {

// Binary morphology over a SilhouetteFrame using a 4-connected (cross)
// structuring element. Out-of-bounds neighbors are treated as background,
// which is the correct behavior for a camera frame edge.
//
// erode:  a pixel survives only if it and all in-bounds 4-neighbors are set.
//         Shrinks blobs and deletes anything smaller than the kernel
//         (single-pixel / thin-line sensor noise).
// dilate: a pixel is set if it or any in-bounds 4-neighbor is set.
//         Grows blobs back and fills small gaps.
//
// open  = erode then dilate -> removes small noise blobs without
//         significantly changing the size of large ones.
// close = dilate then erode -> fills small holes inside a blob.
[[nodiscard]] SilhouetteFrame erode(const SilhouetteFrame& input, std::uint8_t iterations = 1);
[[nodiscard]] SilhouetteFrame dilate(const SilhouetteFrame& input, std::uint8_t iterations = 1);
[[nodiscard]] SilhouetteFrame morphological_open(const SilhouetteFrame& input, std::uint8_t iterations = 1);
[[nodiscard]] SilhouetteFrame morphological_close(const SilhouetteFrame& input, std::uint8_t iterations = 1);

} // namespace fallguard
