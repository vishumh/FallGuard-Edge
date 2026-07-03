#include "fallguard/core.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

namespace {

void draw_rect(fallguard::ImageFrame& frame,
               std::uint16_t x,
               std::uint16_t y,
               std::uint16_t width,
               std::uint16_t height)
{
    for (std::uint16_t py = y; py < y + height; ++py) {
        for (std::uint16_t px = x; px < x + width; ++px) {
            frame.set(px, py, 255);
        }
    }
}

void vision_applies_threshold_and_ignore_zones()
{
    fallguard::FallGuardConfig config{};
    config.frame_width = 8;
    config.frame_height = 8;
    config.silhouette_threshold = 100;
    assert(config.add_ignore_zone({0, 0, 2, 2}));

    fallguard::ImageFrame frame(8, 8, 0);
    frame.set(1, 1, 255);
    frame.set(4, 4, 255);
    frame.set(5, 5, 50);

    fallguard::VisionEngine vision(config);
    const auto silhouette = vision.extract_silhouette(frame);

    assert(!silhouette.at(1, 1));
    assert(silhouette.at(4, 4));
    assert(!silhouette.at(5, 5));
}

void classifier_separates_upright_and_lying_shapes()
{
    fallguard::FallGuardConfig config{};
    config.frame_width = 32;
    config.frame_height = 32;
    config.min_foreground_pixels = 4;

    fallguard::VisionEngine vision(config);
    fallguard::TinyMlEngine classifier(config);

    fallguard::ImageFrame upright(32, 32, 0);
    draw_rect(upright, 14, 6, 4, 18);
    assert(classifier.classify(vision.extract_silhouette(upright)).pose == fallguard::PoseClass::Upright);

    fallguard::ImageFrame lying(32, 32, 0);
    draw_rect(lying, 5, 20, 20, 4);
    assert(classifier.classify(vision.extract_silhouette(lying)).pose == fallguard::PoseClass::Lying);
}

void morphology_open_removes_salt_noise_but_keeps_large_blob()
{
    fallguard::SilhouetteFrame frame(16, 16);
    // A solid 6x6 "person" blob.
    for (std::uint16_t y = 4; y < 10; ++y) {
        for (std::uint16_t x = 4; x < 10; ++x) {
            frame.set(x, y, true);
        }
    }
    // Isolated single-pixel noise, disconnected from the blob.
    frame.set(0, 0, true);
    frame.set(15, 15, true);
    frame.set(1, 14, true);

    const auto opened = fallguard::morphological_open(frame, 1);

    assert(!opened.at(0, 0));
    assert(!opened.at(15, 15));
    assert(!opened.at(1, 14));
    // The interior of the solid blob must survive a single open() pass.
    assert(opened.at(6, 6));
    assert(opened.at(7, 7));
}

void connected_components_finds_largest_blob_and_labels()
{
    fallguard::SilhouetteFrame frame(10, 10);
    // Small blob: 2 pixels.
    frame.set(0, 0, true);
    frame.set(1, 0, true);
    // Large blob: 3x3 = 9 pixels, diagonally adjacent to the small blob's
    // neighborhood so 8-connectivity matters for this test to be meaningful.
    for (std::uint16_t y = 5; y < 8; ++y) {
        for (std::uint16_t x = 5; x < 8; ++x) {
            frame.set(x, y, true);
        }
    }

    std::vector<std::int32_t> labels;
    const auto components = fallguard::find_connected_components(
        frame, fallguard::Connectivity::Eight, &labels);

    assert(components.size() == 2);

    const auto* best = fallguard::largest_component(components);
    assert(best != nullptr);
    assert(best->pixel_count == 9);
    assert(best->bounds.x == 5 && best->bounds.y == 5);
    assert(best->bounds.width == 3 && best->bounds.height == 3);

    const auto best_index = static_cast<std::int32_t>(best - components.data());
    assert(labels[static_cast<std::size_t>(5) * frame.width() + 5] == best_index);
    assert(labels[0] != best_index); // the small blob's top-left pixel
}

void image_processor_isolates_largest_blob_and_drops_noise()
{
    fallguard::FallGuardConfig config{};
    config.frame_width = 20;
    config.frame_height = 20;
    config.min_foreground_pixels = 5;
    config.denoise_iterations = 1;

    fallguard::SilhouetteFrame raw(20, 20);
    // Solid "person" blob, big enough to survive one open() pass.
    for (std::uint16_t y = 10; y < 16; ++y) {
        for (std::uint16_t x = 10; x < 16; ++x) {
            raw.set(x, y, true);
        }
    }
    // Isolated single-pixel sensor noise far from the blob.
    raw.set(0, 0, true);

    fallguard::ImageProcessor processor(config);
    const auto processed = processor.process(raw);

    assert(processed.has_subject);
    assert(!processed.silhouette.at(0, 0));
    assert(processed.silhouette.at(12, 12));
    assert(processed.bounds.width > 0 && processed.bounds.height > 0);
}

void image_processor_reports_no_subject_when_scene_is_empty()
{
    fallguard::FallGuardConfig config{};
    config.frame_width = 10;
    config.frame_height = 10;
    config.min_foreground_pixels = 5;

    fallguard::SilhouetteFrame raw(10, 10);
    fallguard::ImageProcessor processor(config);
    const auto processed = processor.process(raw);

    assert(!processed.has_subject);
    assert(processed.pixel_count == 0);
}

void ignore_zones_round_trip_through_string_serialization()
{
    fallguard::FallGuardConfig config{};
    assert(config.add_ignore_zone({1, 2, 3, 4}));
    assert(config.add_ignore_zone({10, 20, 30, 40}));

    const auto text = config.serialize_ignore_zones();
    assert(text == "1,2,3,4;10,20,30,40");

    fallguard::FallGuardConfig restored{};
    assert(restored.load_ignore_zones_from_string(text));
    assert(restored.ignore_zone_count == 2);
    assert(restored.is_ignored(2, 3));
    assert(restored.is_ignored(15, 25));
    assert(!restored.is_ignored(0, 0));

    // Loading a fresh (empty) string clears any existing zones.
    assert(restored.load_ignore_zones_from_string(""));
    assert(restored.ignore_zone_count == 0);

    // Malformed input is rejected and leaves prior state untouched.
    fallguard::FallGuardConfig malformed{};
    assert(malformed.add_ignore_zone({1, 1, 1, 1}));
    assert(!malformed.load_ignore_zones_from_string("not,valid"));
}

void decision_engine_confirms_only_after_timer()
{
    fallguard::FallGuardConfig config{};
    config.fall_confirmation_ms = 3000;
    fallguard::DecisionEngine decision(config);

    fallguard::ClassificationResult lying{};
    lying.pose = fallguard::PoseClass::Lying;

    assert(decision.update(lying, 1000) == fallguard::DetectionState::SuspectedFall);
    assert(decision.update(lying, 3000) == fallguard::DetectionState::SuspectedFall);
    assert(decision.update(lying, 4000) == fallguard::DetectionState::FallConfirmed);

    fallguard::ClassificationResult upright{};
    upright.pose = fallguard::PoseClass::Upright;
    assert(decision.update(upright, 5000) == fallguard::DetectionState::Monitoring);
}

void alert_tracks_confirmed_fall_state()
{
    fallguard::AlertManager alerts;
    alerts.update(fallguard::DetectionState::Monitoring);
    assert(!alerts.is_active());

    alerts.update(fallguard::DetectionState::FallConfirmed);
    assert(alerts.is_active());

    alerts.clear();
    assert(!alerts.is_active());
}

class FakeCameraSource final : public fallguard::CameraFrameSource {
public:
    bool read(fallguard::CapturedFrame& frame) override
    {
        if (frames_read_ > 0) {
            return false;
        }

        frame.image = fallguard::ImageFrame(4, 3, 7);
        frame.timestamp_ms = 250;
        ++frames_read_;
        return true;
    }

    std::uint16_t width() const override
    {
        return 4;
    }

    std::uint16_t height() const override
    {
        return 3;
    }

private:
    int frames_read_{0};
};

void camera_source_abstraction_returns_captured_frames()
{
    FakeCameraSource source;
    fallguard::CapturedFrame frame;

    assert(source.width() == 4);
    assert(source.height() == 3);
    assert(source.read(frame));
    assert(frame.timestamp_ms == 250);
    assert(frame.image.width() == 4);
    assert(frame.image.height() == 3);
    assert(frame.image.at(2, 1) == 7);
    assert(!source.read(frame));
}

} // namespace

int main()
{
    vision_applies_threshold_and_ignore_zones();
    classifier_separates_upright_and_lying_shapes();
    morphology_open_removes_salt_noise_but_keeps_large_blob();
    connected_components_finds_largest_blob_and_labels();
    image_processor_isolates_largest_blob_and_drops_noise();
    image_processor_reports_no_subject_when_scene_is_empty();
    ignore_zones_round_trip_through_string_serialization();
    decision_engine_confirms_only_after_timer();
    alert_tracks_confirmed_fall_state();
    camera_source_abstraction_returns_captured_frames();
    return 0;
}
