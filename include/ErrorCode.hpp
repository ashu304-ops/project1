// include/ErrorCode.hpp
#pragma once

enum class ErrorCode {
    SUCCESS = 0,
    COIL_INVALID_NEGATIVE_CURRENT,
    COIL_OVERCURRENT_CLAMPED,
    SENSOR_READ_FAILURE
};