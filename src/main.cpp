#include <iostream>

#include "SuspensionController.hpp"
#include "SensorReader.hpp"
#include "DampingCalculator.hpp"
#include "CoilController.hpp"
#include "TelemetryLogger.hpp"
#include "SportStrategy.hpp"
#include "TelemetryRecord.hpp"
#include "TelemetryFormatter.hpp"


int main()
{
    SensorReader sensorReader;
    SportStrategy sportStrategy;

    //DampingCalculator dampingCalculator;

    CoilController coilController;
    TelemetryLogger telemetryLogger;

    SuspensionController suspensionController(
        sensorReader,
        sportStrategy,
        coilController,
        telemetryLogger);

    suspensionController.runControlCycle();

    const TelemetryRecord& record =
        telemetryLogger.lastRecord();

    // Format telemetry as a string
    std::string message =
        TelemetryFormatter::format(record);

    std::cout << message << '\n';

    std::cout << "Suspension control cycle completed.\n";

    return 0;
}