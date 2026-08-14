#pragma once

#include "SensorReader.hpp"
#include "DampingStrategy.hpp"
#include "CoilController.hpp"
#include "TelemetryLogger.hpp"

class SuspensionController
{
public:
    explicit SuspensionController(
        const DampingStrategy& dampingStrategy);

    void runControlCycle();

private:
    SensorReader sensorReader_;
    const DampingStrategy& dampingStrategy_;
    CoilController coilController_;
    TelemetryLogger telemetryLogger_;
};