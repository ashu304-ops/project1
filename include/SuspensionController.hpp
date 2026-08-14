#pragma once

#include "SensorReader.hpp"
#include "DampingCalculator.hpp"
#include "CoilController.hpp"
#include "TelemetryLogger.hpp"

class SuspensionController
{
public:
    void runControlCycle();

private:
    SensorReader sensorReader_;
    DampingCalculator dampingCalculator_;
    CoilController coilController_;
    TelemetryLogger telemetryLogger_;
};
