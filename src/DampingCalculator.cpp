#include "DampingCalculator.hpp"

float DampingCalculator::calculateForceN(float accelG) const noexcept {
    return accelG * 100.0f;
}

DampingCommand DampingCalculator::calculate(const SensorData& sensorData) const {
    const float force = calculateForceN(sensorData.accelerationG);
    return DampingCommand{force, force / 1000.0f};
}