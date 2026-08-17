#pragma once

#include "SensorData.hpp"
#include "DampingCommand.hpp"

enum class DriveMode { Comfort, Sport };

class IDampingStrategy {
public:
    virtual ~IDampingStrategy() = default;
    virtual float calculateForceN(float accelG) const noexcept = 0;
    virtual DampingCommand calculate(const SensorData& sensorData) const = 0;
};