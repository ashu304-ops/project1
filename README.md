
```text
~/MR_Suspension/README.md
```

````markdown
# MR Suspension Controller — SRP Demonstration

A small C++17 project demonstrating the **Single Responsibility Principle (SRP)** using an example of an MR-fluid suspension controller.

The project models a simplified embedded suspension system that:

1. Reads accelerometer data.
2. Calculates the required damping.
3. Controls the MR-fluid coil current.
4. Records telemetry.
5. Coordinates the complete control cycle.

The main goal is not to simulate real vehicle suspension physics. The goal is to demonstrate **clean C++ architecture, separation of responsibilities, object ownership, and testability**.

---

## 1. Engineering Problem

A prototype suspension controller might initially put everything into one class:

```text
SuspensionSystem
 ├── Read sensor
 ├── Calculate damping
 ├── Control coil
 └── Log telemetry
````

This approach works for a small prototype, but the class now has multiple reasons to change.

For example:

* The accelerometer hardware changes.
* The damping equation changes.
* The coil driver changes.
* The current safety limit changes.
* The telemetry format changes.

These changes are unrelated.

A professional design separates these responsibilities.

---

# 2. SRP — Single Responsibility Principle

The principle can be summarized as:

> A class should have one coherent responsibility and one primary reason to change.

This does **not** mean:

> Every class must contain only one function.

Instead, each class should represent one meaningful responsibility.

In this project:

```text
SensorReader
    ↓
Reads sensor information

DampingCalculator
    ↓
Calculates damping requirements

CoilController
    ↓
Safely controls coil current

TelemetryLogger
    ↓
Records telemetry

SuspensionController
    ↓
Coordinates the complete sequence
```

---

# 3. Project Structure

```text
MR_Suspension/
│
├── include/
│   ├── CoilController.hpp
│   ├── DampingCalculator.hpp
│   ├── DampingCommand.hpp
│   ├── SensorData.hpp
│   ├── SensorReader.hpp
│   ├── SuspensionController.hpp
│   ├── TelemetryLogger.hpp
│   └── TelemetryRecord.hpp
│
├── src/
│   ├── CoilController.cpp
│   ├── DampingCalculator.cpp
│   ├── main.cpp
│   ├── SensorReader.cpp
│   ├── SuspensionController.cpp
│   └── TelemetryLogger.cpp
│
├── tests/
│
└── README.md
```

---

# 4. Header and Source File Organization

The project follows a common C++ convention:

```text
.hpp
 ↓
Declaration / interface

.cpp
 ↓
Implementation
```

For example:

```text
DampingCalculator.hpp
        ↓
declares DampingCalculator

DampingCalculator.cpp
        ↓
implements DampingCalculator
```

This keeps interfaces separate from implementation details.

---

# 5. Data Structures

## SensorData

File:

```text
include/SensorData.hpp
```

```cpp
struct SensorData
{
    float accelerationG{0.0f};
};
```

This structure represents accelerometer data.

It only stores data.

It does not:

* Read hardware.
* Calculate damping.
* Control the coil.
* Log telemetry.

This makes it a simple value object.

---

## DampingCommand

File:

```text
include/DampingCommand.hpp
```

```cpp
struct DampingCommand
{
    float forceNewton{0.0f};
    float coilCurrentAmps{0.0f};
};
```

This represents the output of the damping calculation.

It contains:

```text
forceNewton
    ↓
Required damping force

coilCurrentAmps
    ↓
Requested coil current
```

---

## TelemetryRecord

File:

```text
include/TelemetryRecord.hpp
```

```cpp
struct TelemetryRecord
{
    float accelerationG{0.0f};
    float forceNewton{0.0f};
    float coilCurrentAmps{0.0f};
};
```

This contains the information that should be recorded by the telemetry system.

---

# 6. SensorReader

Files:

```text
include/SensorReader.hpp
src/SensorReader.cpp
```

Responsibility:

> Obtain sensor information.

Interface:

```cpp
class SensorReader
{
public:
    SensorData read();
};
```

Implementation:

```cpp
SensorData SensorReader::read()
{
    return SensorData{5.2f};
}
```

Currently the sensor is simulated.

The value:

```text
5.2 G
```

represents a simulated accelerometer reading.

In real embedded firmware, this class could communicate with:

```text
ADC
SPI
I2C
CAN
MEMS accelerometer
```

The important architectural point is that the rest of the system does not need to know how the sensor is physically accessed.

---

# 7. DampingCalculator

Files:

```text
include/DampingCalculator.hpp
src/DampingCalculator.cpp
```

Responsibility:

> Convert sensor information into a damping command.

The calculation is:

```text
force = acceleration × forcePerG

current = force / forcePerAmp
```

The current implementation uses:

```cpp
constexpr float forcePerG = 100.0f;
constexpr float forcePerAmp = 1000.0f;
```

For an acceleration of:

```text
5.2 G
```

the calculation becomes:

```text
force = 5.2 × 100
      = 520 N

current = 520 / 1000
        = 0.52 A
```

The calculator does not know:

* Which sensor produced the data.
* Which coil will receive the current.
* Where telemetry is stored.

This makes it easy to test independently.

---

# 8. CoilController

Files:

```text
include/CoilController.hpp
src/CoilController.cpp
```

Responsibility:

> Safely apply the requested current to the coil.

The controller defines:

```cpp
static constexpr float maximumCurrentAmps = 3.0f;
```

Therefore:

```text
Requested current = 0.52 A
Actual current    = 0.52 A
```

But if the requested current is:

```text
10.0 A
```

the controller limits it:

```text
Requested current = 10.0 A
Maximum current   = 3.0 A
Actual current    = 3.0 A
```

The safety rule is therefore located in one place.

Other parts of the program should not directly modify:

```cpp
currentAmps_
```

This creates a clear safety boundary around actuator control.

---

# 9. TelemetryLogger

Files:

```text
include/TelemetryLogger.hpp
src/TelemetryLogger.cpp
```

Responsibility:

> Store telemetry information.

It receives:

```cpp
TelemetryRecord
```

and stores the latest record:

```cpp
TelemetryRecord lastRecord_{};
```

The logger does not:

* Calculate damping.
* Read sensors.
* Control the coil.

A real embedded implementation could later store telemetry in:

```text
RAM buffer
Flash
SD card
CAN message
UART
Ethernet
external logging system
```

The rest of the system would not need to implement those storage details.

---

# 10. SuspensionController

Files:

```text
include/SuspensionController.hpp
src/SuspensionController.cpp
```

Responsibility:

> Coordinate the control sequence.

The controller owns:

```cpp
SensorReader sensorReader_;
DampingCalculator dampingCalculator_;
CoilController coilController_;
TelemetryLogger telemetryLogger_;
```

The control cycle is:

```text
SensorReader
     │
     │ read()
     ▼
SensorData
     │
     ▼
DampingCalculator
     │
     │ calculate()
     ▼
DampingCommand
     │
     ▼
CoilController
     │
     │ apply()
     ▼
Actual coil current
     │
     ▼
TelemetryLogger
```

The controller coordinates the sequence but does not implement the details of each operation.

---

# 11. Main Program

File:

```text
src/main.cpp
```

The application starts with:

```cpp
int main()
{
    SuspensionController suspensionController;

    suspensionController.runControlCycle();

    return 0;
}
```

The `main()` function therefore has very little knowledge of the internal implementation.

It simply creates the controller and asks it to execute a control cycle.

---

# 12. Object Ownership

The `SuspensionController` directly owns its components:

```cpp
class SuspensionController
{
private:
    SensorReader sensorReader_;
    DampingCalculator dampingCalculator_;
    CoilController coilController_;
    TelemetryLogger telemetryLogger_;
};
```

This means:

```text
SuspensionController
        │
        ├── owns SensorReader
        ├── owns DampingCalculator
        ├── owns CoilController
        └── owns TelemetryLogger
```

No:

```cpp
new
```

or:

```cpp
delete
```

is required.

This provides predictable object lifetime.

When the `SuspensionController` is constructed, its member objects are constructed.

When the controller is destroyed, its member objects are automatically destroyed.

---

# 13. Memory Considerations

This design intentionally avoids dynamic allocation.

There is no:

```cpp
new
delete
malloc
free
```

in the current implementation.

The components are stored directly as members.

This is useful in embedded systems because dynamic allocation can introduce:

* Fragmentation.
* Less predictable allocation time.
* Lifetime-management complexity.
* Failure scenarios when memory is exhausted.

The current design uses small value objects such as:

```cpp
SensorData
DampingCommand
TelemetryRecord
```

These contain only a few `float` values.

---

# 14. Runtime Complexity

The calculations are constant time.

For example:

```cpp
force = accelerationG * forcePerG;
current = force / forcePerAmp;
```

These are:

```text
O(1)
```

The control sequence is also fixed:

```text
Read
 ↓
Calculate
 ↓
Apply
 ↓
Log
```

There are no loops over large collections or dynamically growing containers in the current implementation.

For embedded control software, predictable execution is often important.

---

# 15. Hardware Isolation

One of the important architectural decisions is that hardware access should remain isolated.

Currently:

```text
SensorReader
    ↓
Sensor hardware

CoilController
    ↓
Coil/PWM/DAC hardware
```

while:

```text
DampingCalculator
```

operates only on normal C++ data.

This means the damping algorithm can be tested on a desktop computer without requiring:

```text
Vehicle
Accelerometer
MR-fluid damper
PWM hardware
Microcontroller
```

This is especially useful during development.

---

# 16. Why SRP Improves Testing

Because responsibilities are separated, each component can be tested independently.

For example:

```cpp
DampingCalculator calculator;

SensorData input{5.0f};

DampingCommand result =
    calculator.calculate(input);
```

Expected:

```text
Force = 5.0 × 100
      = 500 N

Current = 500 / 1000
        = 0.5 A
```

The test does not need physical hardware.

Similarly, the coil controller can be tested independently.

For:

```text
Requested current = 10 A
Maximum current   = 3 A
```

the expected result is:

```text
Actual current = 3 A
```

---

# 17. Compile the Project

From the project directory:

```bash
cd ~/MR_Suspension
```

Compile:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o suspension
```

Run:

```bash
./suspension
```

Expected output:

```text
Suspension control cycle completed.
```

---

# 18. Compiler Flags

The project uses:

```text
-std=c++17
```

This tells the compiler to use the C++17 language standard.

```text
-Wall
```

Enables common compiler warnings.

```text
-Wextra
```

Enables additional warnings.

```text
-Iinclude
```

Tells the compiler to search the `include/` directory for header files.

```text
-o suspension
```

Names the resulting executable:

```text
suspension
```

---

# 19. Common C++ Errors Encountered During Development

This project also demonstrates several common beginner C++ errors.

## Missing `#` in include

Incorrect:

```cpp
include "SensorReader.hpp"
```

Correct:

```cpp
#include "SensorReader.hpp"
```

---

## Case sensitivity

C++ is case-sensitive.

These are different:

```cpp
sensorReader_
SensorReader_
```

The project uses:

```cpp
SensorReader sensorReader_;
```

The class name begins with uppercase.

The object/member name begins with lowercase.

---

## Type name versus variable name

Avoid:

```cpp
SensorData SensorData;
```

Prefer:

```cpp
SensorData sensorData;
```

This makes the distinction between the type and object clearer.

---

## Typographical errors

Incorrect:

```cpp
command.coilcurrentAmps
```

Correct:

```cpp
command.coilCurrentAmps
```

Likewise:

```cpp
maximumCurrentAmpsp
```

is incorrect.

Correct:

```cpp
maximumCurrentAmps
```

---

# 20. Dependency Relationships

The important dependencies are:

```text
SensorData
    ↑
SensorReader
```

```text
SensorData + DampingCommand
    ↑
DampingCalculator
```

```text
DampingCommand
    ↑
CoilController
```

```text
TelemetryRecord
    ↑
TelemetryLogger
```

```text
SensorReader
DampingCalculator
CoilController
TelemetryLogger
        ↑
SuspensionController
```

This gives the project a clear dependency structure.

---

# 21. What SRP Prevents

Without SRP:

```text
SuspensionSystem
│
├── sensor hardware
├── sensor validation
├── damping mathematics
├── coil control
├── current safety
├── temperature safety
├── telemetry
└── system coordination
```

This creates a large class with many reasons to change.

With SRP:

```text
SensorReader
     │
     └── sensor responsibility

DampingCalculator
     │
     └── calculation responsibility

CoilController
     │
     └── actuator responsibility

TelemetryLogger
     │
     └── logging responsibility

SuspensionController
     │
     └── coordination responsibility
```

---

# 22. Why We Did Not Use Inheritance

The project does not currently use:

```cpp
virtual
inheritance
interfaces
abstract classes
```

SRP does not require inheritance.

The goal is to first create clear responsibilities.

More advanced abstractions can be introduced later when there is an actual design requirement.

---

# 23. Why We Did Not Use Dynamic Allocation

There is no need for:

```cpp
SensorReader* reader = new SensorReader;
```

The controller can simply own the object:

```cpp
SensorReader sensorReader_;
```

This gives automatic lifetime management.

For a small embedded controller, this is often simpler and more predictable.

---

# 24. Current Limitations

This is an educational project, not production vehicle-control software.

The current implementation uses:

```text
Simulated accelerometer
Simplified damping equation
Simplified coil model
No real PWM/DAC
No temperature sensor
No fault-state machine
No real-time scheduler
No hardware abstraction layer
No unit-testing framework yet
```

A real suspension controller would require considerably more engineering.

---

# 25. Future Improvements

Possible next steps include:

## Testing

Add unit tests for:

```text
SensorReader
DampingCalculator
CoilController
TelemetryLogger
```

A testing framework such as GoogleTest could be introduced.

---

## CMake

Replace the manual compilation command:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o suspension
```

with a proper build system:

```text
CMake
```

---

## Dependency Inversion

The next major architectural improvement is to introduce abstractions for hardware.

For example:

```text
ISensor
    ↑
    ├── RealAccelerometer
    └── FakeAccelerometer
```

and:

```text
ICoilDriver
    ↑
    ├── RealCoilDriver
    └── FakeCoilDriver
```

This allows hardware implementations to be replaced during testing.

This naturally leads toward the:

```text
Dependency Inversion Principle
```

---

# 26. Interview Explanation

A concise interview answer:

> The original suspension system class had multiple responsibilities: sensor acquisition, damping calculation, actuator control, and telemetry. These responsibilities change for different reasons, so I separated them into SensorReader, DampingCalculator, CoilController, and TelemetryLogger. A SuspensionController coordinates these components. This improves testability, reduces coupling, isolates hardware access, and makes requirement changes safer.

---

# 27. Key Takeaways

The most important concepts from this project are:

```text
1. SRP is about responsibility, not number of methods.

2. Different reasons to change usually indicate different responsibilities.

3. Controllers should coordinate rather than implement everything.

4. Hardware access should be isolated.

5. Mathematical logic should be testable without hardware.

6. Prefer direct object ownership when dynamic allocation is unnecessary.

7. Header files declare interfaces.

8. Source files implement those interfaces.

9. C++ is case-sensitive.

10. Small responsibilities make testing and maintenance easier.
```

---

# 28. Architecture Summary

The final architecture is:

```text
                    +----------------------+
                    | SuspensionController |
                    |----------------------|
                    | Coordinate cycle     |
                    +----------+-----------+
                               |
             +-----------------+-----------------+
             |                 |                 |
             v                 v                 v
     +---------------+ +---------------+ +---------------+
     | SensorReader  | |   Damping     | |     Coil      |
     |               | |  Calculator   | |  Controller   |
     +-------+-------+ +-------+-------+ +-------+-------+
             |                 |                 |
             v                 v                 v
        SensorData       DampingCommand     Coil Current
                                                  
                               |
                               v
                       +---------------+
                       | Telemetry     |
                       | Logger        |
                       +---------------+
```

The key idea is:

```text
Read → Calculate → Apply → Log
```

while each class remains responsible for its own part of the system.

```
```
