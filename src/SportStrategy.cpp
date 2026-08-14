#include "SportStrategy.hpp"

DampingCommand SportStrategy::calculate(
    const SensorData& sensorData) const
{
    constexpr float forcePerG = 100.0f;
    constexpr float forcePerAmp = 1000.0f;
    constexpr float sportMultiplier = 0.8f;

    const float force =
        sensorData.accelerationG *
        forcePerG *
        sportMultiplier;

    const float current =
        force / forcePerAmp;

    return DampingCommand{
        force,
        current
    };
}