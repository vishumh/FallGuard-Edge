#include "fallguard/decision_engine.hpp"

namespace fallguard {

DecisionEngine::DecisionEngine(FallGuardConfig config)
    : config_(config)
{
}

DetectionState DecisionEngine::update(const ClassificationResult& result,
                                      std::uint32_t timestamp_ms)
{
    if (result.pose != PoseClass::Lying) {
        reset();
        return state_;
    }

    if (!has_lying_start_) {
        lying_since_ms_ = timestamp_ms;
        has_lying_start_ = true;
        state_ = DetectionState::SuspectedFall;
        return state_;
    }

    const auto elapsed = timestamp_ms - lying_since_ms_;
    state_ = elapsed >= config_.fall_confirmation_ms
        ? DetectionState::FallConfirmed
        : DetectionState::SuspectedFall;

    return state_;
}

DetectionState DecisionEngine::state() const
{
    return state_;
}

void DecisionEngine::reset()
{
    state_ = DetectionState::Monitoring;
    lying_since_ms_ = 0;
    has_lying_start_ = false;
}

const char* to_string(DetectionState state)
{
    switch (state) {
    case DetectionState::Monitoring:
        return "Monitoring";
    case DetectionState::SuspectedFall:
        return "SuspectedFall";
    case DetectionState::FallConfirmed:
        return "FallConfirmed";
    }

    return "Unknown";
}

} // namespace fallguard
