#pragma once

#include "IAccelerometer.hpp"

class SensorReader : public IAccelerometer
{
public:
    SensorData read() override;
};