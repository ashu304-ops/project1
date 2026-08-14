#pragma once

#include "SensorData.hpp"
#include "DampingCommand.hpp"

class DampingStrategy
{
public:
    virtual DampingCommand calculate(
        const SensorData& sensorData) const = 0;

    virtual ~DampingStrategy() = default;
};