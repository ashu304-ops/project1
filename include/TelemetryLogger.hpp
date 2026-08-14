#pragma once

#include "TelemetryRecord.hpp"

class TelemetryLogger
{
public:
    void record(const TelemetryRecord& record);

    const TelemetryRecord& lastRecord() const;

private:
    TelemetryRecord lastRecord_{};
};
