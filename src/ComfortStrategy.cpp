#include "ComfortStrategy.hpp"

DampingCommand ComfortStrategy::calculate(
    const SensorData& sensorData) const
{
    constexpr float forcePerG = 100.0f;
    constexpr float forcePerAmp = 1500.0f;

    const float force =
        sensorData.accelerationG * forcePerG;

    const float current =
        force / forcePerAmp;

    return DampingCommand{
        force,
        current
    };
}