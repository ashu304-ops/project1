#pragma once

#include "ITelemetryLogger.hpp"

class TelemetryLogger : public ITelemetryLogger
{
public:
    void record(const TelemetryRecord& record) override;

    const TelemetryRecord& lastRecord() const;

private:
    TelemetryRecord lastRecord_{};
};