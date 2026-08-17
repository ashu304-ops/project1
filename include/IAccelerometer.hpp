#pragma once

#include "SensorData.hpp"
#include "ErrorCode.hpp"

class IAccelerometer
{
public:
    virtual ErrorCode read(SensorData& outData) = 0;
    virtual ~IAccelerometer() = default;
};