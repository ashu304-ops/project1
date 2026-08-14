#include <iostream>

#include "SuspensionController.hpp"

int main()
{
    SuspensionController suspensionController;

    suspensionController.runControlCycle();

    std::cout << "Suspension control cycle completed.\n";

    return 0;
}
