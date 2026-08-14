#include "CoilController.hpp"

void CoilController::apply(const DampingCommand& command)
{
    float safeCurrent = command.coilCurrentAmps;

    if (safeCurrent < 0.0f)
    {
        safeCurrent = 0.0f;
    }

    if (safeCurrent > maximumCurrentAmps)
    {
        safeCurrent = maximumCurrentAmps;
    }

    currentAmps_ = safeCurrent;
}

float CoilController::current() const
{
    return currentAmps_;
}
