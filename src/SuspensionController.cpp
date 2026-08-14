#include "SuspensionController.hpp"

SuspensionController::SuspensionController(const DampingStrategy& dampingStrategy):dampingStrategy_(dampingStrategy)//class variable intialization 
{

}

void SuspensionController::runControlCycle()
{
    const SensorData sensorData =
        sensorReader_.read();

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
