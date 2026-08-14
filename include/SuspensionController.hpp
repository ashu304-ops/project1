#pragma once

#include "IAccelerometer.hpp"
#include "DampingStrategy.hpp"
#include "ICoilDriver.hpp"
#include "ITelemetryLogger.hpp"

class SuspensionController
{
public:
    SuspensionController(
        IAccelerometer& sensor,
        DampingStrategy& dampingStrategy,
        ICoilDriver& coilController,
        ITelemetryLogger& telemetryLogger);

    void runControlCycle();

private:
    IAccelerometer& sensor_;
    DampingStrategy& dampingStrategy_;
    ICoilDriver& coilController_;
    ITelemetryLogger& telemetryLogger_;
};