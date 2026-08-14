#pragma once

#include "DampingStrategy.hpp"

class SportStrategy : public DampingStrategy
{
public:
    DampingCommand calculate(
        const SensorData& sensorData) const override;
};