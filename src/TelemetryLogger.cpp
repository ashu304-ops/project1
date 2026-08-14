#include "TelemetryLogger.hpp"

void TelemetryLogger::record(const TelemetryRecord& record)
{
    lastRecord_ = record;
}

const TelemetryRecord& TelemetryLogger::lastRecord() const
{
    return lastRecord_;
}