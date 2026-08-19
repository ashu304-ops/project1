#include <iostream>
#include <cassert>
#include "EdgeIotPublisher.hpp"

int main() {
    EdgeIotPublisher publisher("stm32-suspension-edge-01");

    std::string payload = publisher.serialize(1.5f, 450.0f, 1.2f, 32.5f, false);
    
    std::cout << "[IoT Edge Payload Generated]:\n" << payload << "\n";

    // Basic assertions to verify structure
    assert(payload.find("stm32-suspension-edge-01") != std::string::npos);
    assert(payload.find("\"accel_g\":1.50") != std::string::npos);
    assert(payload.find("\"safe_mode\":false") != std::string::npos);

    std::cout << "[PASS] Edge IoT Telemetry Serialization Verified Successfully\n";
    return 0;
}