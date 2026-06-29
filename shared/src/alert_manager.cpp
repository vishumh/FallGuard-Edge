#include "fallguard/alert_manager.hpp"

namespace fallguard {

void AlertManager::update(DetectionState state)
{
    active_ = state == DetectionState::FallConfirmed;
}

void AlertManager::clear()
{
    active_ = false;
}

bool AlertManager::is_active() const
{
    return active_;
}

} // namespace fallguard
