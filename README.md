Embedded MR Fluid Suspension Controller

## Project Overview

This project teaches professional C++ and embedded-engineering thinking through a simplified MR-fluid suspension controller.

The controller receives road-bump data, calculates damping behavior, safely controls an MR-fluid coil, and records telemetry.

```text
Accelerometer
      |
      v
SensorReader
      |
      v
SuspensionController
      |
      +--> DampingCalculator
      |
      +--> CoilDriver
      |
      +--> TelemetryLogger
```

The project is intentionally small.

It does not include:

- Databases.
- Networking.
- Cloud services.
- Microservices.
- Complex frameworks.
- Vehicle communication protocols.
- Production high-voltage hardware.

The purpose is to learn C++ fundamentals, embedded design, testing, memory, performance, and engineering judgment.

***

## System Behavior

A simplified control cycle is:

```text
1. Read acceleration from the road sensor.
2. Validate the sensor value.
3. Estimate bump intensity.
4. Select the driving mode.
5. Calculate required damping.
6. Convert damping into coil current.
7. Apply safe current limits.
8. Monitor temperature and actuator status.
9. Record telemetry.
10. Enter a safe state if a critical failure occurs.
```

Example:

```text
Acceleration: 5.2 g
Driving mode: Sport
Requested current: 2.4 A
Coil temperature: 80 °C
Result: Apply current and log telemetry
```

If the coil is overheated:

```text
Acceleration: 5.2 g
Requested current: 2.4 A
Coil temperature: 150 °C
Result: Disable or reduce current and report an error
```

***

## Learning Philosophy

The goal is not to memorize C++ syntax.

The goal is to develop this engineering chain:

```text
Real problem
    ->
Why the problem matters
    ->
Bad design
    ->
Failure of the bad design
    ->
Better design
    ->
C++ feature
    ->
Memory behavior
    ->
Performance
    ->
Concurrency
    ->
Testing
    ->
Production usage
```

For each topic, we examine:

- Why an engineer needs the concept.
- What a naïve implementation looks like.
- How requirements make the naïve design difficult.
- How a better design improves the system.
- What happens in memory.
- How the design affects performance.
- How it can be tested without real hardware.
- How to explain the choice in an interview.

***

## Hardware Model

The project uses a simple embedded hardware model.

### Accelerometer

Measures suspension movement or road impact.

Example value:

```cpp
5.2f
```

This may represent acceleration in units of \(g\).

### MR-Fluid Coil

Receives electrical current.

Higher current increases the magnetic field, causing the MR fluid to become more resistant to movement.

Example:

```text
0.5 A -> soft response
1.5 A -> medium response
2.5 A -> firm response
```

### Temperature Sensor

Measures coil temperature.

The controller must reduce or disable current if the coil becomes too hot.

### Telemetry Storage

Stores values such as:

```text
Timestamp
Acceleration
Bump intensity
Driving mode
Requested current
Applied current
Temperature
Error status
```

A real implementation might use internal flash, EEPROM, or a ring buffer.

***

## Initial Data Types

The project uses small structures to carry related data.

```cpp
struct SensorData
{
    float accelerationG{0.0f};
    float temperatureCelsius{0.0f};
};

struct DampingCommand
{
    float forceNewton{0.0f};
    float requestedCurrentAmps{0.0f};
};

struct TelemetryRecord
{
    float accelerationG{0.0f};
    float forceNewton{0.0f};
    float requestedCurrentAmps{0.0f};
    float appliedCurrentAmps{0.0f};
    float temperatureCelsius{0.0f};
};
```

These structures are data containers.

They do not read hardware, calculate formulas, or write to storage.

***

## Main Components

### SensorReader

Responsibility:

```text
Read sensor hardware and produce validated sensor data.
```

Possible operations:

```cpp
class SensorReader
{
public:
    SensorData read();
};
```

It should handle sensor-specific concerns such as:

- ADC reading.
- SPI or I2C communication.
- Timeout detection.
- Range validation.
- Sensor status reporting.

It should not calculate damping force.

***

### DampingCalculator

Responsibility:

```text
Convert sensor information into a damping command.
```

Possible operation:

```cpp
class DampingCalculator
{
public:
    DampingCommand calculate(
        const SensorData& sensorData
    ) const;
};
```

It should not:

- Read hardware.
- Set PWM registers.
- Write telemetry.
- Handle flash storage.

This allows the mathematical algorithm to be tested on a normal computer.

***

### DampingStrategy

Responsibility:

```text
Represent one damping behavior or driving mode.
```

Example interface:

```cpp
class DampingStrategy
{
public:
    virtual float calculateCurrent(
        float bumpIntensity
    ) const = 0;

    virtual ~DampingStrategy() = default;
};
```

Possible implementations:

```text
ComfortStrategy
SportStrategy
TrackStrategy
OffRoadStrategy
```

Each strategy applies different behavior.

***

### CoilDriver

Responsibility:

```text
Safely apply current to the MR-fluid coil.
```

Possible interface:

```cpp
class ICoilDriver
{
public:
    virtual CoilResult setCurrent(
        float amps
    ) = 0;

    virtual ~ICoilDriver() = default;
};
```

The driver should enforce:

- Minimum current.
- Maximum current.
- Temperature protection.
- Hardware failure handling.
- PWM or DAC conversion.
- Current feedback validation.

The controller should not directly write coil registers.

***

### TelemetryLogger

Responsibility:

```text
Store or transmit suspension telemetry.
```

Possible interface:

```cpp
class ITelemetryLogger
{
public:
    virtual bool record(
        const TelemetryRecord& record
    ) = 0;

    virtual ~ITelemetryLogger() = default;
};
```

The logger should not determine damping behavior.

It should record what happened.

***

### SuspensionController

Responsibility:

```text
Coordinate the control cycle.
```

Typical sequence:

```text
Read
    ->
Validate
    ->
Calculate
    ->
Apply
    ->
Log
```

It should not contain all hardware and algorithm details.

***

## SOLID Principles Covered

### Single Responsibility Principle

Separate:

```text
Sensor reading
Damping calculation
Coil control
Telemetry logging
System coordination
```

A class should have one coherent responsibility and one primary reason to change. [github](https://github.com/fx-biocoder/solid-in-cpp)

### Open/Closed Principle

Add new damping behavior without repeatedly modifying a large conditional function.

```text
DampingStrategy
    |
    +-- ComfortStrategy
    +-- SportStrategy
    +-- TrackStrategy
    +-- OffRoadStrategy
```

### Liskov Substitution Principle

A derived strategy must honor the behavior expected from the base strategy.

A passive shock absorber should not inherit from an interface that requires it to change damping if it cannot perform that operation.

### Interface Segregation Principle

Prefer focused interfaces:

```text
IAccelerometer
ICoilDriver
ITelemetryLogger
IStatusDisplay
```

Do not force every device to implement every operation.

### Dependency Inversion Principle

The controller should depend on interfaces:

```text
SuspensionController -> ICoilDriver
```

rather than concrete hardware:

```text
SuspensionController -> PwmCoilDriver
```

This allows mock hardware during testing.

***

## Object-Oriented Programming Topics

### Encapsulation

Protect the coil’s internal state:

```cpp
class MagnetCoil
{
public:
    bool setCurrent(float amps);

    float current() const;

private:
    float currentAmps_{0.0f};
};
```

Other code should not be able to do this:

```cpp
coil.currentAmps_ = 500.0f;
```

The coil object validates changes and preserves a safe state.

### Inheritance

Use inheritance only when the derived type genuinely satisfies the base type’s contract.

```text
DampingStrategy
    |
    +-- ComfortStrategy
    +-- SportStrategy
```

### Polymorphism

The controller can use different strategies through one interface:

```cpp
const DampingStrategy& strategy;
strategy.calculateCurrent(bumpIntensity);
```

The actual implementation may be Comfort, Sport, Track, or Off-Road.

### Abstraction

Expose what the controller needs while hiding hardware details.

```cpp
class ICoilDriver
{
public:
    virtual CoilResult setCurrent(float amps) = 0;
};
```

The controller does not need to know how PWM registers are configured.

***

## Error Model

The system should not silently ignore failures.

Example status types:

```cpp
enum class SensorStatus
{
    Valid,
    Timeout,
    InvalidValue,
    HardwareFailure
};
```

```cpp
enum class CoilResult
{
    Success,
    CurrentLimited,
    OverTemperature,
    OverCurrent,
    HardwareFailure
};
```

```cpp
enum class LogResult
{
    Stored,
    StorageFull,
    WriteFailure
};
```

The controller should decide how the complete system responds.

Example:

```text
Sensor timeout
    -> Set coil current to safe value
    -> Record failure
    -> Enter degraded mode

Coil overtemperature
    -> Disable or reduce current
    -> Record temperature
    -> Notify safety logic

Telemetry storage full
    -> Continue safety control
    -> Drop or compress noncritical logs
    -> Record storage error if possible
```

Safety-critical behavior should not depend entirely on successful telemetry logging.

***

## Memory Rules

For every object, ask:

```text
Where is it stored?
Who owns it?
Who destroys it?
How long does it live?
Can it leak?
Can it become dangling?
Can multiple threads access it?
```

### Preferred initial ownership

```cpp
class SuspensionController
{
private:
    SensorReader sensorReader_;
    DampingCalculator calculator_;
    CoilDriver coilDriver_;
    TelemetryLogger logger_;
};
```

This uses direct composition.

Advantages:

- No `new`.
- No `delete`.
- Clear lifetime.
- Predictable memory.
- No ownership ambiguity.

### Non-owning dependency references

```cpp
class SuspensionController
{
public:
    SuspensionController(IAccelerometer& sensor,
                         ICoilDriver& coil,
                         ITelemetryLogger& logger);

private:
    IAccelerometer& sensor_;
    ICoilDriver& coil_;
    ITelemetryLogger& logger_;
};
```

The controller uses these objects but does not destroy them.

The caller must ensure they remain alive.

***

## Performance Rules

The control loop may have a deadline:

```text
Complete control decision within 1 ms
```

For each operation, consider:

- Time complexity.
- Space complexity.
- Dynamic allocation.
- Copying.
- Cache behavior.
- Interrupt interference.
- Lock contention.
- Hardware latency.
- Logging latency.

The main control path should usually avoid:

- Unbounded loops.
- Dynamic allocation.
- Large string creation.
- Blocking storage operations.
- Long mutex waits.
- Unpredictable I/O.

Telemetry can often be placed into a fixed-size buffer and written later.

***

## Concurrency Model

A possible system has:

```text
Sensor task      -> Reads acceleration
Control task     -> Calculates damping
Actuator task    -> Applies coil current
Logger task      -> Stores telemetry
```

Shared data creates risks:

```cpp
float bumpIntensity;
```

If one task writes while another reads, the program may contain a data race.

Possible solutions:

- Atomic values for simple data.
- Mutexes for grouped data.
- Message queues.
- Double buffering.
- Fixed-size event queues.
- Processing data only at control-cycle boundaries.

The design must consider what happens if a sensor update arrives while the calculator is using the previous value.

***

## Testing Strategy

### Unit tests

Test one component at a time:

```text
DampingCalculator
CoilDriver safety limits
Sensor validation
Telemetry formatting
Mode selection
```

### Integration tests

Test a group of components:

```text
SensorReader
    ->
SuspensionController
    ->
FakeCoilDriver
```

### Hardware-in-the-loop tests

Use actual hardware interfaces with controlled test inputs.

### Fault-injection tests

Simulate:

```text
Sensor timeout
Invalid acceleration
Coil overheating
Overcurrent
Logger storage full
Mode change during operation
```

### Safety tests

Verify that dangerous conditions produce safe actuator behavior:

```text
Overtemperature -> current becomes zero
Invalid sensor   -> no unsafe current command
Overcurrent      -> output is limited
```

***

## Planned Learning Phases

### Phase 1: SOLID Principles

Completed topics:

- Single Responsibility Principle.
- Open/Closed Principle.
- Liskov Substitution Principle.
- Interface Segregation Principle.
- Dependency Inversion Principle.

### Phase 2: Four OOP Pillars

Next topics:

1. Encapsulation.
2. Inheritance.
3. Polymorphism.
4. Abstraction.

### Phase 3: Strings and Memory

Topics:

- `std::string`.
- `std::string_view`.
- `char[]`.
- `const char*`.
- `c_str()`.
- Lifetime.
- Stack and heap memory.

### Phase 4: Error Handling

Topics:

- Error codes.
- Exceptions.
- `std::optional`.
- `std::expected`.
- `noexcept`.
- RAII.
- Embedded exception decisions.

### Phase 5: Standard Library Collections

Topics:

```text
std::vector
std::array
std::list
std::set
std::unordered_set
std::map
std::unordered_map
std::queue
std::priority_queue
```

Each will be connected to suspension data or events.

### Phase 6: Equality and Hashing

Use:

```cpp
TelemetryRecord
```

Study:

- `operator==`.
- `std::hash`.
- Hash consistency.
- `unordered_map`.
- `unordered_set`.

### Phase 7: Templates

Build:

```cpp
template<typename T>
class CircularBuffer;
```

Use it for:

```cpp
CircularBuffer<float>
CircularBuffer<TelemetryRecord>
```

### Phase 8: Modern C++

Study:

```text
auto
const
constexpr
enum class
range-based for
lambda
smart pointers
move semantics
RAII
nullptr
structured bindings
optional
variant
string_view
```

Each feature will be introduced through a real engineering problem.

### Phase 9: Smart Pointers and RAII

Study:

```text
std::unique_ptr
std::shared_ptr
std::weak_ptr
```

Focus on:

- Ownership.
- Lifetime.
- Leaks.
- Double deletion.
- Dangling pointers.
- Resource cleanup.

### Phase 10: Move Semantics

Use a large telemetry report to compare:

```text
Copying
Moving
Ownership transfer
Moved-from state
```

### Phase 11: Multithreading

Study:

```text
Race conditions
Mutex
lock_guard
unique_lock
Atomic
condition_variable
Deadlock
Thread-safe queues
```

### Phase 12: Embedded Memory

Study:

```text
Stack
Heap
Static storage
Alignment
Padding
sizeof
Object lifetime
```

### Phase 13: Performance

Analyze:

```text
10,000 sensor readings
Peak bump
Average bump
1 ms control deadline
Container choice
Cache behavior
Allocation behavior
```

### Phase 14: Debugging

Investigate:

```text
Dangling pointer
Memory leak
Double delete
Race condition
Deadlock
Incorrect virtual destructor
Iterator invalidation
Shallow copy
Incorrect move constructor
Use-after-move
```

### Phase 15: Design Patterns

Use only when a real problem requires them:

```text
Strategy
Factory
Observer
Builder
State
```

### Phase 16: Mini Project

Build the embedded MR-fluid suspension controller with:

- Sensor input.
- Bump detection.
- Frequency estimation.
- Driving modes.
- Damping algorithms.
- Coil-current control.
- Temperature protection.
- Telemetry.
- Sensor timeout handling.
- Actuator failure handling.
- Dynamic mode switching.
- Concurrent events.
- Unit testing.

***

## Project Design Principles

Keep these principles visible while studying:

1. Prefer simple designs first.
2. Separate hardware from algorithms.
3. Make ownership explicit.
4. Avoid unnecessary dynamic allocation.
5. Validate data at system boundaries.
6. Keep safety rules close to the hardware they protect.
7. Make failures visible.
8. Avoid hidden global state.
9. Measure performance on the target hardware.
10. Design for testing from the beginning.
11. Use inheritance only when substitution is valid.
12. Prefer composition when behavior is assembled from components.
13. Do not use a feature merely because C++ provides it.
14. Optimize only after identifying a real bottleneck.
15. Treat timing and memory as functional requirements.

***

## Current Architecture

The current conceptual architecture is:

```text
                 +----------------------+
                 | SuspensionController|
                 +----------+-----------+
                            |
       +--------------------+--------------------+
       |                    |                    |
       v                    v                    v
IAccelerometer       DampingStrategy       ICoilDriver
       |                    |                    |
       v                    v                    v
Real Sensor       Comfort/Sport/Track      PWM Driver
Fake Sensor       Off-Road Strategy        Fake Driver

                            |
                            v
                    ITelemetryLogger
                            |
                            v
                    Memory Logger
                    Fake Logger
```

The controller coordinates the system.

The interfaces isolate hardware.

The strategies isolate changing damping behavior.

The concrete implementations can be replaced for testing.

***

## Study Checklist

Before moving to the next topic, you should understand:

- Why one large `SuspensionSystem` class becomes difficult.
- Why responsibilities should be separated.
- Why new damping modes can use strategies.
- Why derived classes must honor base-class expectations.
- Why giant interfaces force meaningless methods.
- Why high-level code should depend on abstractions.
- Who owns each object.
- Whether a pointer or reference is owning or non-owning.
- How to test without real high-voltage hardware.
- Why control logic and telemetry storage may need different timing behavior.

# Next Topic

The next topic is **Encapsulation**.

We will start with this engineering problem:

```cpp
class MagnetCoil
{
public:
    float currentAmps;
};
```

Any part of the suspension software could do this:

```cpp
coil.currentAmps = 500.0f;
```

That could command an unsafe current while the coil is already overheating.

We will then protect the coil’s state using controlled operations, validation, and a clear interface.
