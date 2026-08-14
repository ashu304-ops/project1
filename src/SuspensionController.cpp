#include "SuspensionController.hpp"

void SuspensionController::runControlCycle()
{
    const SensorData sensorData =
        sensorReader_.read();

    const DampingCommand command =
        dampingCalculator_.calculate(sensorData);

    coilController_.apply(command);

    const TelemetryRecord record{
        sensorData.accelerationG,
        command.forceNewton,
        coilController_.current()
    };

    telemetryLogger_.record(record);
}
