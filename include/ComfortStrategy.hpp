#pragma once

#include "IDampingStrategy.hpp"

class ComfortStrategy : public IDampingStrategy {
public:
    float calculateForceN(float accelG) const noexcept override;
    DampingCommand calculate(const SensorData& sensorData) const override;
};