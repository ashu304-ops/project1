#pragma once

#include "ErrorTypes.hpp"

struct TelemetryFrame {
    float accelerationG{0.0f};
    float currentAmps{0.0f};
    SensorError sensorError{SensorError::None};
    CoilError coilError{CoilError::None};
    bool isSafeMode{false};
};