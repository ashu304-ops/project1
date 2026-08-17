#pragma once

constexpr float MAX_SAFE_TEMP_CELSIUS = 85.0f;
constexpr float MAX_SAFE_CURRENT_AMPS = 5.0f;
constexpr float MIN_SAFE_CURRENT_AMPS = 0.0f;

enum class ErrorCode {
    SUCCESS = 0,
    Success = 0,
    COIL_INVALID_NEGATIVE_CURRENT,
    COIL_OVERCURRENT_CLAMPED,
    SENSOR_READ_FAILURE,
    HardwareFault,
    Timeout,
    InvalidParameter
};

enum class SensorError {
    None,
    Timeout,
    ReadTimeout,
    InvalidValue,
    OutOfRange,
    HardwareFault
};

enum class CoilError {
    None,
    OverCurrent,
    OverTemperature,
    HardwareFault
};

struct SensorReadResult {
    SensorError error{SensorError::None};
    float accelerationG{0.0f};

    [[nodiscard]] constexpr bool success() const noexcept {
        return error == SensorError::None;
    }
};

struct CoilResult {
    CoilError error{CoilError::None};
    float actualCurrentAmps{0.0f};

    [[nodiscard]] constexpr bool success() const noexcept {
        return error == CoilError::None;
    }
};