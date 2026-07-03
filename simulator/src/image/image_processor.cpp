#include "fallguard/image/image_processor.hpp"

#include "fallguard/image/connected_components.hpp"
#include "fallguard/image/morphology.hpp"

namespace fallguard {

ImageProcessor::ImageProcessor(FallGuardConfig config)
    : config_(config)
{
}

ProcessedSilhouette ImageProcessor::process(const SilhouetteFrame& raw_silhouette) const
{
    ProcessedSilhouette result;
    result.silhouette = SilhouetteFrame(raw_silhouette.width(), raw_silhouette.height());

    const SilhouetteFrame denoised = config_.denoise_iterations > 0
        ? morphological_open(raw_silhouette, config_.denoise_iterations)
        : raw_silhouette;

    std::vector<std::int32_t> labels;
    const auto components = find_connected_components(denoised, Connectivity::Eight, &labels);
    const auto* best = largest_component(components);

    if (best == nullptr || best->pixel_count < config_.min_foreground_pixels) {
        return result;
    }

    // Find this component's index so we can filter the label map for it.
    const auto best_index = static_cast<std::int32_t>(best - components.data());

    for (std::uint16_t y = 0; y < denoised.height(); ++y) {
        for (std::uint16_t x = 0; x < denoised.width(); ++x) {
            const auto index = static_cast<std::size_t>(y) * denoised.width() + x;
            if (labels[index] == best_index) {
                result.silhouette.set(x, y, true);
            }
        }
    }

    result.bounds = best->bounds;
    result.pixel_count = best->pixel_count;
    result.has_subject = true;
    return result;
}

} // namespace fallguard
