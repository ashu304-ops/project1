#include "CoilController.hpp"

void CoilController::apply(const DampingCommand& command)
{
    currentAmps_ = command.coilCurrentAmps;
}

float CoilController::current() const
{
    return currentAmps_;
}