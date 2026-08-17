#pragma once

#include "ISensorReader.hpp"

class SensorReader : public ISensorReader {
public:
    explicit SensorReader(float initialG = 0.0f) noexcept;

    SensorReadResult read() noexcept override;

    // Test Injection Helpers
    void injectHardwareReading(float gForce) noexcept;
    void injectTimeout(bool timedOut) noexcept;

private:
    float readHardware() const noexcept;
    bool communicationTimedOut() const noexcept;

    float currentG_{0.0f};
    bool timedOut_{false};
};