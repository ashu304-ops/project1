#include "SportStrategy.hpp"

float SportStrategy::calculateForceN(float accelG) const noexcept {
    return accelG * 100.0f * 0.8f;
}

DampingCommand SportStrategy::calculate(const SensorData& sensorData) const {
    const float force = calculateForceN(sensorData.accelerationG);
    return DampingCommand{force, force / 1000.0f};
}