#include <iostream>

#include "SuspensionController.hpp"
#include "SensorReader.hpp"
#include "DampingCalculator.hpp"
#include "CoilController.hpp"
#include "TelemetryLogger.hpp"

int main()
{
    SensorReader sensorReader;
    DampingCalculator dampingCalculator;
    CoilController coilController;
    TelemetryLogger telemetryLogger;

    SuspensionController suspensionController(
        sensorReader,
        dampingCalculator,
        coilController,
        telemetryLogger);

    suspensionController.runControlCycle();

    std::cout << "Suspension control cycle completed.\n";

    return 0;
}