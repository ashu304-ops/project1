```text
~/BE_project/project1/README.md
```

````markdown
# Real-Time MR-Fluid Semi-Active Suspension Control System

## Embedded + C++ + FreeRTOS + STM32 + Arduino Nano + Web Telemetry

A real-time semi-active suspension control system designed around an MR-fluid / electromagnetic damping concept.

The project combines:

- STM32 embedded control
- FreeRTOS real-time task scheduling
- Arduino Nano interface / actuator-side hardware
- C++ suspension-control software
- Sensor acquisition
- Signal filtering
- Damping-force calculation
- Coil-current control
- Thermal safety protection
- Safe-mode handling
- Lock-free telemetry communication
- 100 Hz control loop
- 10 Hz telemetry output
- Node.js WebSocket telemetry bridge
- React/Vite real-time monitoring dashboard
- Unit testing
- CMake-based native build system

The system is structured to demonstrate how a real-time suspension controller can acquire acceleration information, calculate a required damping force, convert that force into an electromagnetic coil-current command, apply safety limits, and expose live operating data to a monitoring interface.

---

# 1. Project Objective

The primary objective is to implement a real-time semi-active suspension controller capable of responding to changing suspension acceleration.

The controller follows the basic chain:

```text
Suspension / Acceleration Sensor
              |
              v
        Sensor Acquisition
              |
              v
        Signal Filtering
              |
              v
       Damping Strategy
              |
              v
       Damping Force
              |
              v
    Force -> Coil Current
              |
              v
       Coil Driver / MR Damper
              |
              v
      Physical Damping Force
````

The controller simultaneously monitors:

```text
Acceleration
Force
Requested Current
Applied Current
Temperature
Safety State
```

These values are transferred to the telemetry system and displayed in real time.

---

# 2. System Architecture

The complete system is organized into several layers.

```text
                         VEHICLE / SUSPENSION
                                |
                                v
                       Acceleration Sensor
                                |
                                v
                     +---------------------+
                     |       STM32         |
                     |                     |
                     |   Sensor Interface  |
                     |        +            |
                     |   FreeRTOS Tasks    |
                     |        +            |
                     | Control Algorithm   |
                     +----------+----------+
                                |
                                | Control Data
                                v
                     +---------------------+
                     |  Arduino Nano /     |
                     |  Hardware Interface |
                     +----------+----------+
                                |
                                v
                     Electromagnetic Coil
                                |
                                v
                       MR Fluid Damper
                                |
                                v
                       Suspension Force


              Telemetry / Monitoring Path
              ----------------------------

 STM32 / Embedded Controller
              |
              v
        Telemetry Frame
              |
              v
      C++ Control Software
              |
              v
      Lock-Free Ring Buffer
              |
              v
      Telemetry Worker
              |
              v
       Node.js WebSocket
            Bridge
              |
              v
       ws://localhost:3001
              |
              v
          React UI
              |
              v
     Real-Time Dashboard
```

---

# 3. Repository Structure

The project root is:

```text
project1/
```

Recommended structure:

```text
project1/
│
├── CMakeLists.txt
├── README.md
├── build/
│
├── include/
│   ├── CoilController.hpp
│   ├── CoilDriver.hpp
│   ├── ErrorTypes.hpp
│   ├── ICoilDriver.hpp
│   ├── IDampingStrategy.hpp
│   ├── ISensorReader.hpp
│   ├── ITelemetryLogger.hpp
│   ├── LockFreeRingBuffer.hpp
│   ├── RingBuffer.hpp
│   ├── SensorReader.hpp
│   ├── SignalFilter.hpp
│   ├── SuspensionController.hpp
│   └── TelemetryFrame.hpp
│
├── src/
│   ├── CoilDriver.cpp
│   ├── SensorReader.cpp
│   ├── SuspensionController.cpp
│   ├── TelemetryLogger.cpp
│   └── main.cpp
│
├── tests/
│   └── ...
│
├── frontend/
│   ├── package.json
│   ├── package-lock.json
│   ├── server.cjs
│   ├── index.html
│   ├── vite.config.js
│   │
│   ├── public/
│   │
│   └── src/
│       ├── App.jsx
│       ├── App.css
│       ├── index.css
│       └── main.jsx
│
├── stm32/
│   ├── ...
│   └── FreeRTOS configuration / firmware
│
└── nano/
    ├── ...
    └── Arduino Nano firmware / interface
```

The exact STM32 and Nano directory names may differ depending on the hardware project layout.

---

# 4. Real-Time Control Requirements

The control system is designed around two primary rates.

## Control Loop

```text
100 Hz
```

Period:

```text
10 ms
```

The control task executes approximately every 10 ms.

Its responsibility is to:

1. Acquire sensor data
2. Filter acceleration
3. Calculate damping force
4. Calculate requested current
5. Apply current through the coil driver
6. Check safety conditions
7. Update controller state
8. Generate telemetry data

---

# 5. Telemetry Rate

Telemetry is intentionally reduced to:

```text
10 Hz
```

This means one telemetry display update approximately every:

```text
100 ms
```

The control algorithm can therefore operate at 100 Hz without unnecessarily flooding the monitoring interface.

Architecture:

```text
Control Loop
    |
    | 100 Hz
    v
Telemetry Queue
    |
    | Worker
    v
10 Hz Display / WebSocket
```

This separation prevents the user interface from becoming part of the real-time control path.

---

# 6. STM32 Implementation

The STM32 is the embedded real-time controller in the hardware architecture.

The STM32 is responsible for deterministic execution of the suspension-control system.

Typical STM32 responsibilities include:

* ADC / sensor acquisition
* GPIO control
* PWM generation
* current-driver control
* temperature measurement
* fault detection
* timing
* FreeRTOS scheduling
* communication
* telemetry generation

The embedded architecture is designed so that safety-critical control is not dependent on the web interface.

The browser is only a monitoring interface.

```text
Browser OFFLINE
      |
      X
      |
STM32 + Controller
      |
      v
Suspension control
continues independently
```

This is important for real-time automotive-style control architecture.

---

# 7. FreeRTOS Architecture

FreeRTOS provides deterministic task scheduling on the STM32.

A typical task architecture is:

```text
+--------------------------------------+
|              FreeRTOS                |
|                                      |
|  +-------------------------------+   |
|  | Sensor Task                   |   |
|  | Sensor acquisition            |   |
|  +-------------------------------+   |
|                 |                    |
|                 v                    |
|  +-------------------------------+   |
|  | Suspension Control Task       |   |
|  | 100 Hz                        |   |
|  +-------------------------------+   |
|                 |                    |
|                 v                    |
|  +-------------------------------+   |
|  | Coil / PWM Task               |   |
|  | Current command               |   |
|  +-------------------------------+   |
|                                      |
|  +-------------------------------+   |
|  | Temperature / Safety Task     |   |
|  +-------------------------------+   |
|                                      |
|  +-------------------------------+   |
|  | Telemetry Task                |   |
|  | 10 Hz                         |   |
|  +-------------------------------+   |
|                                      |
+--------------------------------------+
```

The actual priority and stack configuration should match the STM32 firmware configuration used in the project.

---

# 8. Why FreeRTOS Is Used

FreeRTOS separates the different real-time responsibilities.

Without an RTOS, a large embedded loop can become:

```text
read sensor
calculate
print
check temperature
send telemetry
delay
repeat
```

This can introduce timing jitter.

With FreeRTOS:

```text
Sensor Task
      |
      +----> Control Task
      |
      +----> Safety Task
      |
      +----> Telemetry Task
      |
      +----> Communication Task
```

Each task can have its own execution period and priority.

This is particularly useful for suspension control where control timing should remain deterministic.

---

# 9. Arduino Nano

The Arduino Nano is used as a hardware-side interface/controller component of the project.

The Nano can interface with:

* sensor hardware
* actuator driver hardware
* current-control electronics
* serial communication
* prototype suspension hardware

The Nano should not be considered the primary high-level visualization system.

Its purpose is hardware interfacing and prototyping.

A simplified hardware path is:

```text
Sensor
  |
  v
Arduino Nano
  |
  v
Driver / Interface
  |
  v
Electromagnetic Coil
  |
  v
MR Damper
```

Where the STM32 architecture is used, the STM32 remains the primary real-time embedded controller.

---

# 10. Sensor Acquisition

The controller receives acceleration data in units of:

```text
g
```

Example:

```text
0.50 g
1.00 g
1.50 g
2.00 g
2.50 g
```

The acceleration is passed through the sensor interface.

In the native development build, simulated hardware input is also available for software validation.

The current simulation contains:

```cpp
float currentSimulatedG =
    1.5f + 1.0f * std::sin(tick);
```

This produces a changing acceleration signal approximately between:

```text
0.5 g
```

and

```text
2.5 g
```

This allows the complete controller pipeline to be tested without requiring physical suspension excitation during every software test.

---

# 11. Signal Filtering

Raw sensor signals are normally subject to:

* electrical noise
* mechanical vibration
* sensor noise
* ADC quantization
* transient disturbances

Therefore, the controller does not directly use the raw sensor value for damping-force calculation.

The pipeline is:

```text
Raw Acceleration
       |
       v
 Signal Filter
       |
       v
Filtered Acceleration
       |
       v
Damping Strategy
```

The project contains:

```text
SignalFilter
```

for this purpose.

---

# 12. Damping Strategy

The damping strategy converts acceleration into a target damping force.

The controller uses:

```cpp
const float forceN =
    strategy_->calculateForceN(filteredG);
```

This separates the physical control algorithm from the controller infrastructure.

The strategy can therefore be replaced without rewriting:

* sensor acquisition
* telemetry
* coil control
* safety logic
* threading

For example:

```text
Comfort Strategy
Sport Strategy
Soft Strategy
Hard Strategy
Adaptive Strategy
```

can be implemented as different damping strategies.

---

# 13. Damping Force

The telemetry output shows the calculated damping force.

Example:

```text
Acceleration: 2.49 g
Force:        124.34 N
```

Another example:

```text
Acceleration: 0.50 g
Force:        25.12 N
```

The exact relationship is determined by the configured damping strategy and filter.

The force is calculated by the C++ controller rather than the React interface.

This is important:

```text
React does NOT calculate the suspension force.
```

The frontend only displays the controller's calculated value.

---

# 14. Force to Coil Current Conversion

The current implementation converts force to requested coil current using:

```cpp
const float requestedCurrentA =
    forceN * 0.01f;
```

Therefore:

```text
Current = Force × 0.01
```

Example:

```text
Force = 100 N

Current = 100 × 0.01

Current = 1.0 A
```

Another example:

```text
Force = 50 N

Current = 50 × 0.01

Current = 0.5 A
```

This is the control mapping implemented in the current software.

For a calibrated physical MR damper, this mapping should ultimately be replaced or calibrated against measured force-current characteristics.

---

# 15. Coil Driver

The coil driver receives the requested current.

Interface:

```cpp
CoilResult setCurrent(
    float requestedCurrentAmps,
    float temperatureCelsius
) noexcept;
```

The coil driver is responsible for applying the requested current while checking safety conditions.

Telemetry contains:

```text
Requested Current
Applied Current
```

This distinction is important.

For example:

```text
Requested: 1.20 A
Applied:   1.20 A
```

means the commanded current was successfully applied.

If a safety condition occurs, the applied current may differ from the requested current.

---

# 16. Thermal Monitoring

Temperature is now part of the live telemetry pipeline.

The controller maintains:

```text
temperatureCelsius
```

The temperature is passed through:

```text
Controller
      |
      v
Coil Driver
      |
      v
Telemetry
      |
      v
WebSocket
      |
      v
React
```

Example live output:

```text
Temp: 42.4484 C
```

The web dashboard then displays:

```text
TEMPERATURE

42°C
```

The temperature is therefore not hard-coded in the frontend.

It originates from the controller telemetry frame.

---

# 17. Thermal Safety

The suspension controller performs a thermal safety check.

Current controller logic:

```cpp
if (ambientTempC >= 150.0f) {
    enterSafeMode(SensorError::HardwareFault);
    return;
}
```

This establishes a high-temperature safety boundary in the controller.

The coil driver also checks its configured maximum safe temperature.

The safety chain is therefore:

```text
Temperature
     |
     v
Thermal Safety Check
     |
     +------ Normal ------> Coil Current
     |
     |
     +------ Fault -------> SAFE MODE
                              |
                              v
                       Coil Current = 0
```

---

# 18. Safe Mode

When an unsafe condition is detected, the controller enters safe mode.

The implementation calls:

```cpp
enterSafeMode(...)
```

The safe-mode routine disables normal coil current:

```cpp
coil_.setCurrent(0.0f, 25.0f);
```

The telemetry then reports:

```text
SafeMode: YES
```

The React interface displays:

```text
SAFE MODE
```

in the danger color.

During normal operation:

```text
SafeMode: NO
```

and the dashboard displays:

```text
SAFE
```

---

# 19. TelemetryFrame

Telemetry is represented by a structured frame.

The important fields are:

```text
accelerationG
forceNewton
requestedCurrentAmps
appliedCurrentAmps
temperatureCelsius
sensorError
coilError
isSafeMode
```

Conceptually:

```text
TelemetryFrame
│
├── Acceleration
├── Force
├── Requested Current
├── Applied Current
├── Temperature
├── Sensor Error
├── Coil Error
└── Safe Mode
```

This gives the monitoring system a complete snapshot of the controller state.

---

# 20. Lock-Free Telemetry Queue

The project uses:

```text
LockFreeRingBuffer<TelemetryFrame, 256>
```

for communication between the control loop and telemetry worker.

The architecture is:

```text
                 100 Hz
                    |
                    v
          +------------------+
          | Control Thread   |
          +------------------+
                    |
                    v
        Lock-Free Ring Buffer
                    |
                    v
          +------------------+
          | Telemetry Worker |
          +------------------+
                    |
                    v
                  10 Hz
```

The objective is to avoid blocking the real-time control thread with console or network operations.

This is an important real-time design principle.

---

# 21. Native C++ Control Application

The project also provides a native C++ version of the control system.

The main executable is:

```text
build/project1
```

and the deployed application used by the bridge is:

```text
suspension_app
```

Build:

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

Then:

```bash
cp build/project1 suspension_app
```

Run:

```bash
./suspension_app
```

Expected startup:

```text
[SYSTEM] Initializing suspension hardware...
[SYSTEM] Spawning Telemetry Worker Thread...
[SYSTEM] Spawning Control Loop Thread (100 Hz)...
[SYSTEM] Running. Press ENTER to initiate clean shutdown...
```

---

# 22. C++ Control Threads

The native application contains two main threads.

## Control Thread

```text
100 Hz
```

Responsibilities:

```text
Generate/read sensor data
        |
        v
Run controller
        |
        v
Generate TelemetryFrame
        |
        v
Push to lock-free queue
```

## Telemetry Thread

Responsibilities:

```text
Pop TelemetryFrame
        |
        v
Process telemetry
        |
        v
Display every 10 frames
```

Since the control thread runs at 100 Hz:

```text
10 frames = approximately 100 ms
```

therefore:

```text
10 Hz telemetry display
```

---

# 23. Real Telemetry Example

The controller has produced telemetry such as:

```text
[TELEMETRY 10Hz]
Accel: 2.48254 g |
Force: 122.833 N |
Requested: 1.22833 A |
Applied: 1.22833 A |
Temp: 42.4484 C |
SafeMode: NO
```

This demonstrates that the telemetry pipeline is carrying:

```text
Acceleration
Force
Requested Current
Applied Current
Temperature
Safety State
```

as intended.

---

# 24. Node.js Telemetry Bridge

The frontend is connected to the C++ application through:

```text
Node.js WebSocket Bridge
```

File:

```text
frontend/server.cjs
```

The bridge starts:

```text
suspension_app
```

and reads its stdout.

Telemetry lines are parsed by:

```javascript
parseTelemetry(line)
```

The bridge converts the textual C++ telemetry into structured JSON.

Example:

```json
{
  "type": "telemetry",
  "timestamp": 1786993273946,
  "accelerationG": 2.2292,
  "forceNewton": 107.78,
  "requestedCurrentAmps": 1.0778,
  "appliedCurrentAmps": 1.0778,
  "temperatureCelsius": 25,
  "safeMode": false
}
```

---

# 25. WebSocket Communication

The telemetry bridge listens on:

```text
ws://localhost:3001
```

Health endpoint:

```text
http://localhost:3001/health
```

Health test:

```bash
curl http://localhost:3001/health
```

Expected:

```json
{"status":"running"}
```

WebSocket architecture:

```text
C++ suspension_app
        |
        | stdout
        v
Node.js Bridge
        |
        | JSON
        v
WebSocket :3001
        |
        v
React Frontend
```

---

# 26. React Frontend

The frontend is implemented using:

```text
React
Vite
CSS
WebSocket
```

Main component:

```text
frontend/src/App.jsx
```

The React application creates:

```javascript
new WebSocket("ws://localhost:3001");
```

When telemetry arrives:

```javascript
socket.onmessage
```

parses the JSON and updates the React state.

The dashboard then automatically updates.

---

# 27. Real-Time Dashboard

The dashboard displays:

```text
ACCELERATION
DAMPING FORCE
COIL CURRENT
TEMPERATURE
```

and detailed telemetry:

```text
Acceleration
Damping Force
Requested Current
Applied Current
Temperature
```

It also displays:

```text
SYSTEM RUNNING
```

when the WebSocket is connected.

If the bridge disconnects:

```text
DISCONNECTED
```

is displayed.

---

# 28. Current Dashboard Example

A valid live dashboard state can look like:

```text
Real-Time Telemetry Monitor

MR-Fluid Suspension Control System

SYSTEM RUNNING


ACCELERATION
1.02 g

DAMPING FORCE
47.0 N

COIL CURRENT
0.47 A

TEMPERATURE
49 °C


Live Suspension Telemetry

SAFE

Acceleration       1.0232 g
Damping Force      46.96 N
Requested Current  0.4696 A
Applied Current    0.4696 A
Temperature        49.0 °C
```

These values originate from the telemetry pipeline rather than being manually entered into the frontend.

---

# 29. Temperature Telemetry

The temperature system has been integrated into the controller-to-dashboard path.

The complete path is:

```text
Temperature Source
       |
       v
SuspensionController
       |
       v
lastTemperatureC()
       |
       v
TelemetryFrame
       |
       v
Telemetry Worker
       |
       v
C++ stdout
       |
       v
Node.js Bridge
       |
       v
JSON WebSocket
       |
       v
React
       |
       v
Temperature Card
```

Therefore, the displayed temperature can change as the telemetry value changes.

---

# 30. Why Temperature Was Initially Constant

Earlier versions of the native control loop called:

```cpp
controller.runCycle(25.0f);
```

on every control cycle.

That means the controller was receiving:

```text
25.0°C
25.0°C
25.0°C
25.0°C
...
```

Consequently the dashboard correctly displayed:

```text
25°C
```

because the backend was actually sending 25°C.

The frontend itself was not responsible for the constant temperature.

Once the temperature input was changed to a dynamic value, telemetry correctly showed values such as:

```text
42.4484°C
```

and the dashboard displayed approximately:

```text
42°C
```

and later:

```text
49°C
```

This confirms that temperature is now flowing through the telemetry path.

---

# 31. Data Validation

The system should be validated at multiple levels.

## Level 1 — C++ Controller

Verify:

```text
Acceleration changes
Force changes
Current changes
Temperature changes
SafeMode changes correctly
```

## Level 2 — Telemetry

Verify:

```text
TelemetryFrame contains current values
```

## Level 3 — Node.js

Verify:

```bash
curl http://localhost:3001/health
```

and inspect:

```text
[BRIDGE] Telemetry:
```

## Level 4 — WebSocket

Verify that the browser receives:

```json
{
    "type": "telemetry"
}
```

## Level 5 — React

Verify:

```text
Acceleration updates
Force updates
Current updates
Temperature updates
Safe status updates
```

---

# 32. Build Instructions

From the project root:

```bash
cd ~/BE_project/project1
```

Configure:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build -j$(nproc)
```

Check executable:

```bash
ls -lh build/project1
```

Copy executable:

```bash
cp build/project1 suspension_app
```

Run:

```bash
./suspension_app
```

---

# 33. Unit Tests

The project includes a unit-test target.

Build:

```bash
cmake --build build -j$(nproc)
```

The build should show:

```text
[ 50%] Built target project1
[100%] Built target unit_tests
```

Run the tests using the generated test executable or CTest configuration.

If CTest is configured:

```bash
cd build
ctest --output-on-failure
```

---

# 34. Frontend Installation

Move into the frontend:

```bash
cd frontend
```

Install dependencies:

```bash
npm install
```

The frontend uses:

```text
React
React DOM
ws
Vite
ESLint
```

The `ws` dependency is required by:

```text
server.cjs
```

---

# 35. Start the Backend Bridge

From:

```text
project1/frontend
```

run:

```bash
node server.cjs
```

Expected:

```text
[BRIDGE] Project root: /home/.../project1
[BRIDGE] Application: /home/.../project1/suspension_app
[BRIDGE] Starting suspension application...

======================================
 Suspension Telemetry Bridge
======================================
 WebSocket: ws://localhost:3001
 Health:    http://localhost:3001/health
======================================
```

The bridge automatically starts:

```text
suspension_app
```

---

# 36. Start the React Frontend

In another terminal:

```bash
cd ~/BE_project/project1/frontend
npm run dev
```

Vite normally provides a development URL such as:

```text
http://localhost:5173
```

Open that address in the browser.

---

# 37. Complete Software Startup

The easiest development sequence is:

### Terminal 1

```bash
cd ~/BE_project/project1
cmake -S . -B build
cmake --build build -j$(nproc)
cp build/project1 suspension_app
```

### Terminal 2

```bash
cd ~/BE_project/project1/frontend
node server.cjs
```

### Terminal 3

```bash
cd ~/BE_project/project1/frontend
npm run dev
```

Then open the Vite URL.

---

# 38. System Startup Flow

Complete runtime:

```text
             POWER / START
                   |
                   v
             STM32 START
                   |
                   v
             FreeRTOS
                   |
                   v
        Initialize peripherals
                   |
                   v
          Sensor acquisition
                   |
                   v
          Control calculation
                   |
                   v
          Damping force
                   |
                   v
          Coil current command
                   |
                   v
        Safety / temperature
                   |
                   v
            Telemetry
                   |
                   v
             C++ system
                   |
                   v
         Lock-free telemetry
                   |
                   v
           Node.js bridge
                   |
                   v
           WebSocket :3001
                   |
                   v
            React / Vite
                   |
                   v
          Engineer Dashboard
```

---

# 39. Real-Time Design Principle

The web interface is NOT part of the safety-critical control loop.

The correct architecture is:

```text
             SAFETY / CONTROL PATH
             ---------------------

Sensor
  |
  v
STM32
  |
  v
FreeRTOS
  |
  v
Control Algorithm
  |
  v
Coil Driver
  |
  v
MR Damper


             MONITORING PATH
             ---------------

Controller
    |
    v
Telemetry
    |
    v
Node.js
    |
    v
WebSocket
    |
    v
Browser
```

If the browser crashes:

```text
Suspension control should continue.
```

If Node.js stops:

```text
Suspension control should continue.
```

If the dashboard is closed:

```text
Suspension control should continue.
```

The monitoring layer is intentionally separated from the control layer.

---

# 40. Safety Philosophy

The controller implements defensive behavior for abnormal conditions.

Important states include:

```text
NORMAL
SAFE MODE
SENSOR ERROR
COIL ERROR
OVER-TEMPERATURE
```

When a critical error occurs:

```text
Normal Control
      |
      v
Fault detected
      |
      v
Safe Mode
      |
      v
Coil current disabled
```

This prevents the software from continuing normal actuation when a safety condition has been detected.

---

# 41. Mechanical Interpretation

From a mechanical-engineering perspective, the system represents a semi-active damping architecture.

The basic relationship is:

```text
Suspension excitation
        |
        v
Acceleration
        |
        v
Control law
        |
        v
Desired damping force
        |
        v
Electromagnetic current
        |
        v
MR-fluid rheological response
        |
        v
Variable damping force
```

Increasing electromagnetic excitation changes the MR fluid's effective resistance to motion.

Therefore the controller can change damping behavior without using a conventional continuously powered hydraulic actuator.

---

# 42. MR Fluid Concept

Magnetorheological fluid contains magnetically responsive particles.

When an electromagnetic field is applied:

```text
Magnetic field increases
        |
        v
Particle structures form
        |
        v
Fluid apparent yield stress increases
        |
        v
Damping resistance increases
```

When the magnetic field is reduced:

```text
Magnetic field decreases
        |
        v
Particle structure weakens
        |
        v
Damping resistance decreases
```

The coil current therefore becomes the control input for variable damping.

---

# 43. Semi-Active Nature

This system is semi-active rather than a fully active suspension.

A fully active system can inject mechanical energy into the suspension.

A semi-active MR damper primarily changes the amount of damping force generated by the damper.

Conceptually:

```text
Passive Damper
    |
    | fixed damping
    v
Mechanical suspension


Semi-active MR Damper
    |
    | variable damping
    v
Controller
    |
    v
Coil Current
```

The controller modifies damping characteristics based on measured suspension behavior.

---

# 44. Current Telemetry Relationship

An example operating point:

```text
Acceleration = 1.0232 g
Force        = 46.96 N
Current      = 0.4696 A
Temperature  = 49.0 °C
SafeMode     = NO
```

The current mapping is:

```text
46.96 N × 0.01
=
0.4696 A
```

This is consistent with the implemented software mapping.

---

# 45. Thermal Behavior

The temperature telemetry should eventually represent the physical coil temperature rather than only ambient temperature.

A physical implementation can use:

```text
NTC thermistor
RTD
temperature IC
ADC temperature sensor
digital temperature sensor
```

A realistic thermal model can also be implemented.

For example:

```text
Electrical power
      |
      v
Coil heating
      |
      v
Thermal mass
      |
      v
Temperature rise
      |
      v
Cooling to environment
```

A first-order thermal approximation can be represented as:

```text
dT/dt =
(P - (T - Tambient)/Rthermal)
/
Cthermal
```

where:

```text
P          = coil electrical power
T          = coil temperature
Tambient   = ambient temperature
Rthermal   = thermal resistance
Cthermal   = thermal capacitance
```

This should be calibrated against physical measurements for an actual damper.

---

# 46. Physical Validation

For real hardware validation, the following measurements are recommended:

## Acceleration

Use a calibrated accelerometer.

Compare:

```text
Sensor output
vs
Reference accelerometer
```

## Force

Use a load cell.

Compare:

```text
Calculated damping force
vs
Measured damper force
```

## Current

Use:

```text
Current shunt
Hall-effect current sensor
```

Compare:

```text
Requested current
vs
Measured coil current
```

## Temperature

Use a calibrated temperature sensor.

Compare:

```text
Telemetry temperature
vs
Measured coil temperature
```

---

# 47. Force-Current Calibration

The current implementation uses a simple mapping:

```text
I = F × 0.01
```

For a physical MR damper, the relationship should be obtained experimentally.

A test procedure can be:

```text
Current = 0.0 A
Measure Force

Current = 0.1 A
Measure Force

Current = 0.2 A
Measure Force

...

Current = maximum safe current
Measure Force
```

Generate:

```text
Force vs Current
```

Then fit a calibration curve.

The software can then use:

```text
Desired Force
       |
       v
Calibration Model
       |
       v
Required Current
```

rather than a simple linear multiplier.

---

# 48. Sensor Calibration

The acceleration sensor should be calibrated for:

```text
offset
scale
axis alignment
noise
sampling rate
temperature drift
```

The filter should then be tuned based on actual sensor characteristics.

---

# 49. Timing Verification

The control loop is intended to operate at:

```text
100 Hz
```

which corresponds to:

```text
10 ms
```

The telemetry display operates at:

```text
10 Hz
```

which corresponds to:

```text
100 ms
```

Timing should be measured on the target STM32 hardware using:

```text
GPIO timing pin
DWT cycle counter
hardware timer
logic analyzer
oscilloscope
```

This verifies actual task jitter and execution time.

---

# 50. FreeRTOS Timing Verification

For STM32 FreeRTOS implementation, verify:

```text
Control task period
Sensor task execution time
Coil control execution time
Telemetry task execution time
Task priorities
Stack usage
CPU utilization
Interrupt latency
```

FreeRTOS runtime statistics can be used to monitor task execution.

---

# 51. Fault Testing

The following faults should be deliberately tested.

## Sensor Failure

Expected:

```text
Safe Mode
```

## Coil Driver Failure

Expected:

```text
Safe Mode
```

## Over-temperature

Expected:

```text
Safe Mode
```

## Communication Failure

Expected:

```text
Control continues
```

The web dashboard should not be required for safe control.

---

# 52. Communication Failure Test

Stop the frontend:

```text
Browser OFF
```

The controller should continue.

Stop Node.js:

```text
Node bridge OFF
```

The embedded controller should continue.

This validates separation between:

```text
Control
```

and:

```text
Monitoring
```

---

# 53. Development Environment

The native software has been tested in a Linux development environment.

Example:

```text
Arch Linux
GCC / C++
CMake
Node.js
npm
Vite
React
WebSocket
```

The embedded STM32 target uses:

```text
STM32
FreeRTOS
C/C++
```

The Arduino Nano side uses:

```text
Arduino-compatible firmware
```

---

# 54. Dependencies

## Native C++

```text
C++ compiler
CMake
pthread / standard threading support
```

## Frontend

```text
Node.js
npm
React
React DOM
Vite
ws
```

## Embedded

```text
STM32 toolchain
STM32 HAL / LL as applicable
FreeRTOS
STM32CubeMX / STM32CubeIDE if used
```

## Nano

```text
Arduino toolchain
Arduino Nano board package
```

---

# 55. Frontend Dependency Installation

From:

```bash
cd ~/BE_project/project1/frontend
```

run:

```bash
npm install
```

Check:

```bash
npm list
```

Build frontend:

```bash
npm run build
```

Run development server:

```bash
npm run dev
```

---

# 56. Backend Health Check

Check the bridge:

```bash
curl -v http://localhost:3001/health
```

Expected:

```text
HTTP/1.1 200 OK
```

and:

```json
{"status":"running"}
```

Check port:

```bash
ss -ltnp | grep ':3001'
```

Expected:

```text
LISTEN ... :3001 ... node
```

---

# 57. Debugging WebSocket Connection

If React reports:

```text
The connection to ws://localhost:3001/
was interrupted while the page was loading.
```

check:

```bash
ss -ltnp | grep ':3001'
```

then:

```bash
curl http://localhost:3001/health
```

If health returns:

```json
{"status":"running"}
```

the Node bridge is running.

Then verify:

```text
node server.cjs
```

is still running.

Do not terminate the bridge while the browser is using it.

---

# 58. Frontend WebSocket Address

The React application connects using:

```javascript
new WebSocket("ws://localhost:3001");
```

This assumes:

```text
Browser
and
Node bridge
```

are running on the same machine.

For a remote system, replace `localhost` with the appropriate server address and configure the network appropriately.

---

# 59. Production Architecture

A possible physical production architecture is:

```text
              VEHICLE
                 |
       +---------+---------+
       |                   |
 Accelerometer        Temperature
       |                   |
       +---------+---------+
                 |
                 v
             STM32 MCU
                 |
              FreeRTOS
                 |
       +---------+---------+
       |                   |
       v                   v
Control Task          Safety Task
       |
       v
Damping Algorithm
       |
       v
Current Command
       |
       v
PWM / Current Driver
       |
       v
MR Coil
       |
       v
MR Damper
```

Telemetry:

```text
STM32
  |
  v
CAN / UART / USB / Ethernet
  |
  v
Gateway
  |
  v
Node.js
  |
  v
WebSocket
  |
  v
Engineering Dashboard
```

---

# 60. Important Engineering Limitation

The current native software demonstrates the complete control and telemetry architecture.

However, a production automotive suspension system requires substantially more validation.

In particular:

```text
Force-current calibration
Sensor calibration
Thermal characterization
Current-driver characterization
Damper characterization
EMI/EMC testing
Fault injection
Real-time timing verification
Mechanical durability
Environmental testing
Electrical safety
Functional safety
```

must be completed before treating the controller as a production automotive safety system.

The software telemetry being valid does not by itself prove that the physical damper is safe under every operating condition.

---

# 61. Engineering Validation Matrix

| Parameter         | Software        | Hardware         | Required Validation      |
| ----------------- | --------------- | ---------------- | ------------------------ |
| Acceleration      | Yes             | Sensor           | Calibration              |
| Force             | Yes             | Damper/load cell | Force measurement        |
| Requested Current | Yes             | Yes              | Command verification     |
| Applied Current   | Yes             | Yes              | Current measurement      |
| Temperature       | Yes             | Sensor           | Thermal calibration      |
| Safe Mode         | Yes             | Yes              | Fault injection          |
| 100 Hz Control    | Yes             | STM32            | Timing measurement       |
| 10 Hz Telemetry   | Yes             | Yes              | Communication validation |
| MR Fluid Response | Model           | Physical         | Experimental test        |
| Coil Heating      | Model/Telemetry | Physical         | Thermal test             |

---

# 62. Engineering Data Flow

The final data path is:

```text
ACCELERATION
     |
     v
Sensor
     |
     v
Filtering
     |
     v
Damping Strategy
     |
     v
FORCE
     |
     v
Force-to-Current Mapping
     |
     v
REQUESTED CURRENT
     |
     v
Coil Driver
     |
     v
APPLIED CURRENT
     |
     v
MR Damper
     |
     v
Damping Response
```

At the same time:

```text
Temperature
     |
     v
Safety Logic
     |
     v
Safe Mode
```

All values are reported through:

```text
TelemetryFrame
```

---

# 63. Example End-to-End Runtime

Example:

```text
Acceleration:
1.0232 g
```

Controller:

```text
Damping strategy
      |
      v
Force:
46.96 N
```

Current mapping:

```text
46.96 × 0.01
=
0.4696 A
```

Coil driver:

```text
Requested:
0.4696 A

Applied:
0.4696 A
```

Temperature:

```text
49.0 °C
```

Safety:

```text
SafeMode:
NO
```

Telemetry:

```json
{
  "type": "telemetry",
  "accelerationG": 1.0232,
  "forceNewton": 46.96,
  "requestedCurrentAmps": 0.4696,
  "appliedCurrentAmps": 0.4696,
  "temperatureCelsius": 49.0,
  "safeMode": false
}
```

Dashboard:

```text
ACCELERATION      1.02 g
DAMPING FORCE     47.0 N
COIL CURRENT      0.47 A
TEMPERATURE       49 °C
STATUS            SYSTEM RUNNING
SAFETY            SAFE
```

---

# 64. Project Status

Current implemented architecture includes:

* [x] C++ suspension controller
* [x] Sensor interface
* [x] Sensor simulation for native validation
* [x] Signal filtering
* [x] Damping strategy
* [x] Damping-force calculation
* [x] Force-to-current conversion
* [x] Coil-driver abstraction
* [x] Applied-current telemetry
* [x] Temperature telemetry
* [x] Thermal safety logic
* [x] Safe-mode handling
* [x] Telemetry frame
* [x] Lock-free telemetry queue
* [x] 100 Hz control loop
* [x] 10 Hz telemetry output
* [x] CMake build
* [x] Unit-test target
* [x] Node.js telemetry bridge
* [x] WebSocket communication
* [x] Health endpoint
* [x] React dashboard
* [x] Live acceleration display
* [x] Live damping-force display
* [x] Live coil-current display
* [x] Live temperature display
* [x] Safe-mode display
* [x] STM32 architecture
* [x] FreeRTOS architecture
* [x] Arduino Nano hardware-interface architecture

---

# 65. Future Hardware Calibration

The next engineering step is not simply adding more software.

The important step is correlating the software model with measured physical data.

Required measurements:

```text
Acceleration
Force
Current
Temperature
Damper velocity
```

A useful test dataset is:

```text
Time
Acceleration
Velocity
Force
Current
Temperature
```

This can then be used to determine:

```text
F = f(v, I, T)
```

where:

```text
F = damping force
v = damper velocity
I = coil current
T = temperature
```

This is a more physically meaningful MR-damper model than using acceleration alone.

---

# 66. Recommended Physical Control Model

For a more advanced implementation:

```text
Acceleration
      |
      v
State Estimation
      |
      +----> Velocity
      |
      +----> Displacement
      |
      v
Suspension State
      |
      v
Control Strategy
      |
      v
Desired Force
      |
      v
MR Damper Calibration
      |
      v
Required Current
      |
      v
Current Controller
      |
      v
Coil
```

This allows the system to evolve from a demonstration controller into a more physically representative semi-active suspension controller.

---

# 67. Final System Summary

The project implements a complete real-time suspension-control software architecture connecting embedded control, actuator control, telemetry, and engineering visualization.

The complete system is:

```text
                    MR-FLUID SUSPENSION
                           |
                           v
                    Sensor / Hardware
                           |
                           v
                    STM32 + FreeRTOS
                           |
                           v
                 Real-Time Control Loop
                         100 Hz
                           |
                           v
                   Signal Filtering
                           |
                           v
                  Damping Strategy
                           |
                           v
                    Damping Force
                           |
                           v
                 Coil Current Command
                           |
                           v
                  Coil Driver / PWM
                           |
                           v
                    MR Fluid Damper
                           |
                           v
                  Variable Damping
                           |
                           |
                    TELEMETRY
                           |
                           v
                 Lock-Free Queue
                           |
                           v
                  Telemetry Worker
                           |
                           v
                    Node.js Bridge
                           |
                           v
                    WebSocket :3001
                           |
                           v
                     React / Vite
                           |
                           v
                 Engineering Dashboard
```

The system provides real-time visibility into:

```text
Acceleration
Damping Force
Requested Current
Applied Current
Temperature
Safety State
```

while maintaining separation between the real-time control path and the user-interface monitoring path.

---

# 68. Quick Start

From the project root:

```bash
cd ~/BE_project/project1
```

Build:

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
cp build/project1 suspension_app
```

Start backend:

```bash
cd frontend
node server.cjs
```

In another terminal:

```bash
cd ~/BE_project/project1/frontend
npm install
npm run dev
```

Check backend:

```bash
curl http://localhost:3001/health
```

Expected:

```json
{"status":"running"}
```

Open the Vite URL shown by:

```bash
npm run dev
```

The dashboard should show live:

```text
Acceleration
Force
Current
Temperature
Safe Mode
```

---

# 69. Author / Project

Project:

```text
Real-Time MR-Fluid Semi-Active Suspension Control System
```

Architecture:

```text
STM32
FreeRTOS
Arduino Nano
C++
CMake
Node.js
WebSocket
React
Vite
```

Control rates:

```text
100 Hz Control Loop
10 Hz Telemetry
```

Primary engineering objective:

```text
Real-time variable damping control
with safety monitoring and live telemetry.
```

---

````

