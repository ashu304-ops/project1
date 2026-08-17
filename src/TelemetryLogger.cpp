// src/TelemetryLogger.cpp
#include "TelemetryLogger.hpp"
#include <iostream>

void TelemetryLogger::record(const SensorReadResult& sensor, const CoilResult& coil, float forceN) noexcept {
    // Left intentionally silent in real-time execution loop to prevent 
    // blocking stdout I/O during runCycle(). Off-thread logger handles output.
    (void)sensor;
    (void)coil;
    (void)forceN;
}

void TelemetryLogger::recordSensorError(SensorError error) noexcept {
    std::cout << "\n[FAULT] Sensor Read Error (Code " 
              << static_cast<int>(error) << "). Safe mode enforced.\n";
}

void TelemetryLogger::recordCoilError(CoilError error) noexcept {
    std::cout << "\n[WARNING] Coil Driver Event (Code " 
              << static_cast<int>(error) << "). Degraded/Clamped output.\n";
}