#pragma once

#include "DampingStrategy.hpp"

class ComfortStrategy : public DampingStrategy
{
public:
    DampingCommand calculate(
        const SensorData& sensorData) const override;
};