#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cmath>

#include "SensorReader.hpp"
#include "CoilDriver.hpp"
#include "TelemetryLogger.hpp"
#include "SuspensionController.hpp"
#include "LockFreeRingBuffer.hpp"
#include "TelemetryFrame.hpp"

std::atomic<bool> systemRunning{true};
LockFreeRingBuffer<TelemetryFrame, 256> telemetryQueue;

// THREAD 1: Real-Time Control Loop (100 Hz / 10ms cycle)
void controlLoopTask(SuspensionController& controller, SensorReader& sensor, CoilDriver& coil) {
    using namespace std::chrono_literals;

    float tick = 0.0f;

    while (systemRunning.load(std::memory_order_relaxed)) {
        auto nextCycle = std::chrono::steady_clock::now() + 10ms;

        // 1. Simulate dynamic sensor readings (sine wave between 0.5g and 2.5g)
        float currentSimulatedG = 1.5f + 1.0f * std::sin(tick);
        tick += 0.05f;

        // Inject simulated hardware reading into sensor
        sensor.injectHardwareReading(currentSimulatedG);

        // 2. Run control cycle (calculates force & updates coil current)
        controller.runCycle(25.0f);

        // 3. Snapshot actual outputs from hardware abstractions
        float activeCurrent = coil.current();
        
        TelemetryFrame frame{
            currentSimulatedG,  // Actual active G processed
            activeCurrent,      // Actual current set on coil
            SensorError::None,
            CoilError::None,
            false
        };

        // 4. Non-blocking queue push
        telemetryQueue.push(frame);

        std::this_thread::sleep_until(nextCycle);
    }
}

// THREAD 2: Async Telemetry / Display Worker Thread
void telemetryLoggingTask(TelemetryLogger& logger) {
    using namespace std::chrono_literals;
    size_t frameCounter = 0;

    while (systemRunning.load(std::memory_order_relaxed)) {
        while (auto frame = telemetryQueue.pop()) {
            frameCounter++;

            if (frame->isSafeMode) {
                logger.recordSensorError(frame->sensorError);
            }

            // In-place line update every 10 cycles (10 Hz display rate)
            if (frameCounter % 10 == 0) {
                std::cout << "\r[TELEMETRY 10Hz] Accel: " << frame->accelerationG 
                          << " g | Current: " << frame->currentAmps 
                          << " A | SafeMode: " << (frame->isSafeMode ? "YES" : "NO") 
                          << "   " << std::flush;
            }
        }
        std::this_thread::sleep_for(5ms);
    }

    // Drain queue on shutdown
    while (auto frame = telemetryQueue.pop()) {
        if (frame->isSafeMode) {
            logger.recordSensorError(frame->sensorError);
        }
    }
}

int main() {
    std::cout << "[SYSTEM] Initializing suspension hardware...\n";

    SensorReader sensor;
    CoilDriver coil;
    TelemetryLogger logger;
    SuspensionController controller(sensor, coil, logger);

    std::cout << "[SYSTEM] Spawning Telemetry Worker Thread...\n";
    std::thread telemetryThread(telemetryLoggingTask, std::ref(logger));

    std::cout << "[SYSTEM] Spawning Control Loop Thread (100 Hz)...\n";
    std::thread controlThread(controlLoopTask, std::ref(controller), std::ref(sensor), std::ref(coil));

    std::cout << "[SYSTEM] Running. Press ENTER to initiate clean shutdown...\n\n";
    
    std::cin.get();

    std::cout << "\n[SYSTEM] Shutdown signal received. Stopping control threads...\n";
    systemRunning.store(false, std::memory_order_relaxed);

    if (controlThread.joinable()) {
        controlThread.join();
        std::cout << "[SYSTEM] Control thread stopped.\n";
    }

    if (telemetryThread.joinable()) {
        telemetryThread.join();
        std::cout << "[SYSTEM] Telemetry thread stopped.\n";
    }

    std::cout << "[SYSTEM] Shutdown complete cleanly.\n";
    return 0;
}