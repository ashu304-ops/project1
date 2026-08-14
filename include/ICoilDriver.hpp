#pragma once

#include "DampingCommand.hpp"

class ICoilDriver
{
public:
    virtual void apply(const DampingCommand& command) = 0;

    virtual float current() const = 0;

    virtual ~ICoilDriver() = default;
};