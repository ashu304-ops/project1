#pragma once

#include "DampingCommand.hpp"

class CoilController
{
public:
    void apply(const DampingCommand& command);

    float current() const;

private:
    static constexpr float maximumCurrentAmps = 3.0f;

    float currentAmps_{0.0f};
};
