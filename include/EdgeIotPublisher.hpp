#ifndef EDGE_IOT_PUBLISHER_HPP
#define EDGE_IOT_PUBLISHER_HPP

#include <string>
#include <sstream>
#include <iomanip>

struct EdgeTelemetryPacket {
    std::string deviceId;
    unsigned long sequenceNumber;
    float accelerationG;
    float forceNewton;
    float appliedCurrentAmps;
    float temperatureCelsius;
    bool safeMode;
};

class EdgeIotPublisher {
public:
    explicit EdgeIotPublisher(std::string deviceId) 
        : deviceId_(std::move(deviceId)), seqNum_(0) {}

    // Edge-side serialization into a lightweight IoT payload (JSON format)
    std::string serialize(float accel, float force, float current, float temp, bool safeMode) {
        seqNum_++;
        std::ostringstream oss;
        oss << "{"
            << "\"device_id\":\"" << deviceId_ << "\","
            << "\"seq\":" << seqNum_ << ","
            << "\"metrics\":{"
            << "\"accel_g\":" << std::fixed << std::setprecision(2) << accel << ","
            << "\"force_n\":" << force << ","
            << "\"current_a\":" << current << ","
            << "\"temp_c\":" << temp << ","
            << "\"safe_mode\":" << (safeMode ? "true" : "false")
            << "}"
            << "}";
        return oss.str();
    }

private:
    std::string deviceId_;
    unsigned long seqNum_;
};

#endif // EDGE_IOT_PUBLISHER_HPP