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
};

#endif // SUSPENSION_CONTROLLER_HPP