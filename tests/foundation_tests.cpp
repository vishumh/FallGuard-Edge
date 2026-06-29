#include "fallguard/core.hpp"

#include <cassert>
#include <cstdint>

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
    decision_engine_confirms_only_after_timer();
    alert_tracks_confirmed_fall_state();
    camera_source_abstraction_returns_captured_frames();
    return 0;
}
