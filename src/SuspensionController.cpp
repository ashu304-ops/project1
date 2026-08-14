#include "SuspensionController.hpp"

SuspensionController::SuspensionController(
    IAccelerometer& sensor,
    DampingStrategy& dampingStrategy,
    ICoilDriver& coilController,
    ITelemetryLogger& telemetryLogger)
    : sensor_(sensor),
      dampingStrategy_(dampingStrategy),
      coilController_(coilController),
      telemetryLogger_(telemetryLogger)
{
}

void SuspensionController::runControlCycle()
{
    const SensorData sensorData =
        sensor_.read();

    const DampingCommand command =
        dampingStrategy_.calculate(sensorData);

    coilController_.apply(command);

    const TelemetryRecord record{
        sensorData.accelerationG,
        command.forceNewton,
        coilController_.current()
    };

    telemetryLogger_.record(record);
}