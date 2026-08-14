#pragma once

#include "TelemetryRecord.hpp"

class ITelemetryLogger
{
public:
    virtual void record(const TelemetryRecord& record) = 0;

    virtual ~ITelemetryLogger() = default;
};