#include "SuspensionController.hpp"
#include "ComfortStrategy.hpp"

SuspensionController::SuspensionController(ISensorReader& sensor, ICoilDriver& coil, ITelemetryLogger& logger) noexcept
    : sensor_(sensor), coil_(coil), logger_(logger) {}

void SuspensionController::setStrategy(std::unique_ptr<IDampingStrategy> strategy) noexcept {
    strategy_ = std::move(strategy);
}

void SuspensionController::runCycle(float ambientTempC) noexcept {
    // 1. Comprehensive Safety & Thermal Error Handling Check
    if (ambientTempC >= 150.0f) {
        enterSafeMode(SensorError::HardwareFault);
        return;
    }

    if (!strategy_) {
        strategy_ = std::make_unique<ComfortStrategy>();
    }

    // 2. Sensor Fault Error Recovery Path
    const SensorReadResult sensorResult = sensor_.read();
    if (!sensorResult.success()) {
        enterSafeMode(sensorResult.error);
        return;
    }

    const float filteredG = filter_.filter(sensorResult.accelerationG);
    const float forceN = strategy_->calculateForceN(filteredG);

    // Save recent forces in zero-allocation template ring buffer
    forceHistory_.push(forceN);

    // Scaling: 100 N -> 1.0 A
    float requestedCurrentA = forceN * 0.01f; 

    const CoilResult coilResult = coil_.setCurrent(requestedCurrentA, ambientTempC);
    if (coilResult.error != CoilError::None) {
        enterSafeMode(SensorError::HardwareFault);
        return;
    }

    logger_.record(sensorResult, coilResult, forceN);
}

void SuspensionController::enterSafeMode(SensorError error) noexcept {
    coil_.setCurrent(0.0f, 25.0f);
    logger_.recordSensorError(error);
}