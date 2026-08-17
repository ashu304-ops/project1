// src/CoilDriver.cpp
#include "CoilDriver.hpp"

CoilResult CoilDriver::setCurrent(float requestedCurrentAmps, float temperatureCelsius) noexcept {
    // Thermal Shutdown Guardrail
    if (temperatureCelsius >= MAX_SAFE_TEMP_CELSIUS) {
        currentAmps_ = 0.0f;
        return CoilResult{CoilError::OverTemperature, 0.0f};
    }

    // Over-Current Clamping Guardrail
    if (requestedCurrentAmps > MAX_SAFE_CURRENT_AMPS) {
        currentAmps_ = MAX_SAFE_CURRENT_AMPS;
        return CoilResult{CoilError::OverCurrent, currentAmps_};
    }

    // Negative Current Prevention
    if (requestedCurrentAmps < 0.0f) {
        currentAmps_ = 0.0f;
        return CoilResult{CoilError::None, 0.0f};
    }

    currentAmps_ = requestedCurrentAmps;
    return CoilResult{CoilError::None, currentAmps_};
}

ErrorCode CoilDriver::apply(const DampingCommand& command) {
    if (command.coilCurrentAmps < MIN_SAFE_CURRENT_AMPS) {
        currentAmps_ = MIN_SAFE_CURRENT_AMPS;
        return ErrorCode::COIL_INVALID_NEGATIVE_CURRENT;
    }

    if (command.coilCurrentAmps > MAX_SAFE_CURRENT_AMPS) {
        currentAmps_ = MAX_SAFE_CURRENT_AMPS;
        return ErrorCode::COIL_OVERCURRENT_CLAMPED;
    }

    currentAmps_ = command.coilCurrentAmps;
    return ErrorCode::SUCCESS;
}

float CoilDriver::current() const noexcept {
    return currentAmps_;
}