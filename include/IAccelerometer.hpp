#pragma once

#include "SensorData.hpp"

class IAccelerometer
{
public:
    virtual SensorData read() = 0;

    virtual ~IAccelerometer() = default;
};