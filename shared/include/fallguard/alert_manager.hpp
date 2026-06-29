#pragma once

#include "fallguard/decision_engine.hpp"

namespace fallguard {

class AlertManager {
public:
    void update(DetectionState state);
    void clear();

    [[nodiscard]] bool is_active() const;

private:
    bool active_{false};
};

} // namespace fallguard
