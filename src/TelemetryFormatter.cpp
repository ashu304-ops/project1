#include "TelemetryFormatter.hpp"

#include <string>

std::string TelemetryFormatter::format(
    const TelemetryRecord& record)
{
    return "Acceleration: " +
           std::to_string(record.accelerationG) +
           " g, Force: " +
           std::to_string(record.forceNewton) +
           " N, Current: " +
           std::to_string(record.coilCurrentAmps) +
           " A";
}