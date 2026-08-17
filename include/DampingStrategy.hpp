#pragma once

#include "SensorData.hpp"
#include "DampingCommand.hpp"

class DampingStrategy {
public:
    virtual ~DampingStrategy() = default;
    virtual DampingCommand calculate(const SensorData& sensorData) const = 0;
};