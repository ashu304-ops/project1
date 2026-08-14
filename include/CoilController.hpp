#pragma once

#include "ICoilDriver.hpp"

class CoilController : public ICoilDriver
{
public:
    void apply(const DampingCommand& command) override;

    float current() const override;

private:
    float currentAmps_{0.0f};
};