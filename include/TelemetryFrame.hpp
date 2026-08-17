#pragma once

#include "ErrorTypes.hpp"

struct TelemetryFrame {
    float accelerationG{0.0f};
    float forceNewton{0.0f};
    float requestedCurrentAmps{0.0f};
    float appliedCurrentAmps{0.0f};
    float temperatureCelsius{0.0f};

    SensorError sensorError{SensorError::None};
    CoilError coilError{CoilError::None};

    bool isSafeMode{false};
};
