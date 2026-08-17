// include/TelemetryLogger.hpp
#pragma once

#include "ITelemetryLogger.hpp"
#include "ErrorTypes.hpp"

class TelemetryLogger : public ITelemetryLogger {
public:
    void record(const SensorReadResult& sensor, const CoilResult& coil, float forceN) noexcept override;
    void recordSensorError(SensorError error) noexcept override;
    void recordCoilError(CoilError error) noexcept override;
};