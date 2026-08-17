#ifndef SUSPENSION_CONTROLLER_HPP
#define SUSPENSION_CONTROLLER_HPP

#include <memory>
#include "ISensorReader.hpp"
#include "ICoilDriver.hpp"
#include "ITelemetryLogger.hpp"
#include "IDampingStrategy.hpp"
#include "SignalFilter.hpp"
#include "RingBuffer.hpp" // STL Containers & Templates

class SuspensionController {
public:
    SuspensionController(ISensorReader& sensor, ICoilDriver& coil, ITelemetryLogger& logger) noexcept;

    void setStrategy(std::unique_ptr<IDampingStrategy> strategy) noexcept;
    void runCycle(float ambientTempC) noexcept;
    void enterSafeMode(SensorError error) noexcept;

    [[nodiscard]] float lastAccelerationG() const noexcept {
        return lastAccelerationG_;
    }

    [[nodiscard]] float lastForceN() const noexcept {
        return lastForceN_;
    }

    [[nodiscard]] float lastRequestedCurrentA() const noexcept {
        return lastRequestedCurrentA_;
    }

    [[nodiscard]] float lastTemperatureC() const noexcept {
        return lastTemperatureC_;
    }

    [[nodiscard]] SensorError lastSensorError() const noexcept {
        return lastSensorError_;
    }

    [[nodiscard]] CoilError lastCoilError() const noexcept {
        return lastCoilError_;
    }

    [[nodiscard]] bool isSafeMode() const noexcept {
        return safeMode_;
    }

    // Fixed-capacity STL container for offline error telemetry analysis
    [[nodiscard]] const RingBuffer<float, 32>& getRecentForceHistory() const noexcept {
        return forceHistory_;
    }

private:
    ISensorReader& sensor_;
    ICoilDriver& coil_;
    ITelemetryLogger& logger_;
    
    std::unique_ptr<IDampingStrategy> strategy_;
    SignalFilter filter_;
    RingBuffer<float, 32> forceHistory_{}; // Stack/inline allocated STL buffer

    float lastAccelerationG_{0.0f};
    float lastForceN_{0.0f};
    float lastRequestedCurrentA_{0.0f};
    float lastTemperatureC_{25.0f};

    SensorError lastSensorError_{SensorError::None};
    CoilError lastCoilError_{CoilError::None};

    bool safeMode_{false};
};

#endif // SUSPENSION_CONTROLLER_HPP