#include <Arduino.h>

#include "fallguard/core.hpp"

namespace {

fallguard::FallGuardConfig config;
fallguard::DecisionEngine decision(config);
fallguard::AlertManager alerts;

} // namespace

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("FallGuard Edge ESP32 foundation booted");
    Serial.println("Camera, vision capture, and TinyML model integration are planned for later sprints.");
}

void loop()
{
    fallguard::ClassificationResult placeholder{};
    placeholder.pose = fallguard::PoseClass::Empty;

    const auto state = decision.update(placeholder, millis());
    alerts.update(state);

    delay(1000);
}
