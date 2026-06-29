#pragma once

#include "fallguard/config.hpp"
#include "fallguard/tinyml_engine.hpp"

#include <cstdint>

namespace fallguard {

enum class DetectionState {
    Monitoring,
    SuspectedFall,
    FallConfirmed,
};

class DecisionEngine {
public:
    explicit DecisionEngine(FallGuardConfig config);

    [[nodiscard]] DetectionState update(const ClassificationResult& result,
                                        std::uint32_t timestamp_ms);
    [[nodiscard]] DetectionState state() const;
    void reset();

private:
    FallGuardConfig config_{};
    DetectionState state_{DetectionState::Monitoring};
    std::uint32_t lying_since_ms_{0};
    bool has_lying_start_{false};
};

const char* to_string(DetectionState state);

} // namespace fallguard
