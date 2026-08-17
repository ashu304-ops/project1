// src/CoilController.cpp
#include "CoilController.hpp"

ErrorCode CoilController::apply(const DampingCommand& command) {
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

float CoilController::current() const noexcept {
    return currentAmps_;
}