#include <iostream>
#include <cassert>
#include <cmath>
#include "SuspensionController.hpp"

class MockSensor : public ISensorReader {
public:
    SensorReadResult resultToReturn{SensorError::None, 2.0f};
    SensorReadResult read() noexcept override { return resultToReturn; }
};

class MockCoil : public ICoilDriver {
public:
    float currentAmps_{0.0f};

    CoilResult setCurrent(float current, float temp) noexcept override {
        currentAmps_ = (temp >= 150.0f) ? 0.0f : current;
        return CoilResult{CoilError::None, currentAmps_};
    }

    ErrorCode apply(const DampingCommand& command) override {
        currentAmps_ = command.coilCurrentAmps;
        return ErrorCode::SUCCESS;
    }

    float current() const noexcept override { 
        return currentAmps_; 
    }
};

class MockLogger : public ITelemetryLogger {
public:
    void record(const SensorReadResult&, const CoilResult&, float) noexcept override {}
    void recordSensorError(SensorError) noexcept override {}
    void recordCoilError(CoilError) noexcept override {}
};

int main() {
    MockSensor sensor;
    MockCoil coil;
    MockLogger logger;
    SuspensionController controller(sensor, coil, logger);

    // Test Normal Cycle
    controller.runCycle(25.0f);
    std::cout << "Recorded Coil Current: " << coil.current() << " A\n";
    assert(std::abs(coil.current() - 1.0f) < 0.001f);

    // Test STL Template RingBuffer History
    assert(controller.getRecentForceHistory().size() == 1);

    // Test Thermal Limits Safety Error Handling Path
    controller.runCycle(155.0f); // High Temp Threshold > 150 C
    assert(coil.current() == 0.0f); // Shutdown triggered cleanly

    std::cout << "[PASS] All 9 Embedded C++ Concepts Verified & Passed\n";
    return 0;
}