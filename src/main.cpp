#include <iostream>

#include "SuspensionController.hpp"
#include "SnowStrategy.hpp"
using namespace  std;
int main()
{
    SnowStrategy snowStrategy;

    SuspensionController suspensionController(
        snowStrategy);

    suspensionController.runControlCycle();

    cout << "Snow control cycle completed.\n";

    return 0;
}