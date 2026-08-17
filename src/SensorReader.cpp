#include "SensorReader.hpp"

SensorReader::SensorReader(float initialG) noexcept
    : currentG_(initialG) {}

SensorReadResult SensorReader::read() noexcept {
    SensorReadResult result{};

    if (communicationTimedOut()) {
        result.error = SensorError::Timeout;
        result.accelerationG = 0.0f;
        return result;
    }

    const float value = readHardware();
    result.error = SensorError::None;
    result.accelerationG = value;
    return result;
}

float SensorReader::readHardware() const noexcept {
    return currentG_;
}

bool SensorReader::communicationTimedOut() const noexcept {
    return timedOut_;
}

void SensorReader::injectHardwareReading(float gForce) noexcept {
    currentG_ = gForce;
}

void SensorReader::injectTimeout(bool timedOut) noexcept {
    timedOut_ = timedOut;
}