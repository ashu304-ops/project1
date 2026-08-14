#pragma once

#include "SensorData.hpp"
#include "DampingCommand.hpp"

class DampingCalculator
{
public:
    DampingCommand calculate(const SensorData& sensorData) const;
};
