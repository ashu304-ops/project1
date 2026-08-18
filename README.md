

# ⚡ Real-Time MR-Fluid Semi-Active Suspension Control System

> **A real-time embedded suspension controller that turns raw acceleration into intelligent electromagnetic damping — with deterministic control, safety supervision, and live web telemetry.**

## 🏎️ What Is This?

This project implements a **real-time semi-active suspension control architecture** based on an **MR-fluid / electromagnetic damping concept**.

Instead of using a fixed damper characteristic, the controller continuously evaluates suspension acceleration and dynamically adjusts electromagnetic coil current to influence the damping force.

The core idea is simple:

```text
        SENSOR
          │
          ▼
   ┌───────────────┐
   │ Acquisition   │
   └───────┬───────┘
           │
           ▼
   ┌───────────────┐
   │ Signal Filter │
   └───────┬───────┘
           │
           ▼
   ┌───────────────┐
   │ Control Law   │
   └───────┬───────┘
           │
           ▼
   ┌───────────────┐
   │ Desired Force  │
   └───────┬───────┘
           │
           ▼
   ┌───────────────┐
   │ Force → Current│
   └───────┬───────┘
           │
           ▼
   ┌───────────────┐
   │ Coil Driver   │
   └───────┬───────┘
           │
           ▼
      MR DAMPER
           │
           ▼
   Controlled Damping
```

At the same time, the system monitors its own operating conditions and can transition into a **safe mode** when abnormal conditions are detected.

---

# 🎯 Engineering Goals

The project was designed around a few principles:

### ⏱️ Deterministic real-time control

The primary control loop executes at:

**100 Hz → 10 ms control period**

The controller must complete sensor processing, control calculation, safety checks, and actuator command generation within its real-time budget.

### 🛡️ Safety before performance

The controller does not blindly follow the control algorithm.

It continuously considers:

* Coil-current limits
* Temperature limits
* Sensor validity
* Actuator limits
* Fault conditions
* Safe-mode transitions

### 📡 Observable systems

A controller that works but cannot be observed is difficult to debug.

Therefore the system exposes live telemetry at:

**10 Hz → 100 ms telemetry period**

through a Node.js WebSocket bridge and a React/Vite monitoring dashboard.

---

# 🧠 Control Pipeline

The main control path can be summarized as:

```text
Acceleration
     │
     ▼
┌───────────────┐
│ Sensor Input  │
└───────┬───────┘
        │
        ▼
┌────────────────┐
│ Low-pass /     │
│ Signal Filter  │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Suspension     │
│ Control Law    │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Desired Damping│
│ Force          │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Force → Current│
│ Mapping        │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Safety Limits  │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Coil Current   │
│ Command        │
└────────────────┘
```

This separation keeps **sensor processing, control strategy, actuator mapping, and safety logic independently testable**.

---

# ⚙️ Real-Time Architecture

The embedded side is organized around **FreeRTOS tasks** with clear responsibilities.

```text
                 STM32
                   │
        ┌──────────┴──────────┐
        │                     │
        ▼                     ▼
   Control Domain       Monitoring Domain
        │                     │
        ▼                     ▼
 Sensor Acquisition       Telemetry
        │                     │
        ▼                     ▼
 Signal Filtering        Data Snapshot
        │                     │
        ▼                     ▼
 Control Algorithm       Communication
        │
        ▼
 Safety Supervisor
        │
        ▼
 Coil Command
```

### Suggested task responsibilities

| Task               |        Frequency | Responsibility               |
| ------------------ | ---------------: | ---------------------------- |
| `ControlTask`      |       **100 Hz** | Main suspension control loop |
| `SensorTask`       | Sensor-dependent | Acquisition and validation   |
| `SafetyTask`       |         Periodic | Thermal/fault supervision    |
| `TelemetryTask`    |        **10 Hz** | Publish system state         |
| Communication Task |     Event-driven | External interface           |

The architecture intentionally separates the **hard real-time control path** from lower-priority monitoring and communication work.

---

# 🔒 Safety Architecture

A semi-active actuator should never be allowed to command arbitrary current.

The safety layer therefore sits between the control algorithm and the physical actuator:

```text
              Control Algorithm
                     │
                     ▼
              Desired Current
                     │
                     ▼
            ┌─────────────────┐
            │ Safety Manager  │
            ├─────────────────┤
            │ Current Limit   │
            │ Thermal Limit   │
            │ Sensor Validity │
            │ Fault Detection │
            │ Safe Mode       │
            └────────┬────────┘
                     │
             ┌───────┴───────┐
             │               │
           SAFE            NORMAL
             │               │
             ▼               ▼
        Safe Current     Requested
          Command         Current
```

If the system detects an unsafe operating condition, the controller can override the requested command and transition toward a predictable safe state.

### Example safety conditions

```text
Temperature too high
        │
        ▼
Current derating / safe mode

Invalid sensor
        │
        ▼
Fallback control / safe command

Over-current request
        │
        ▼
Command saturation

Critical fault
        │
        ▼
SAFE MODE
```

This is deliberately designed so that **safety logic can override performance logic**.

---

# 🌡️ Thermal Protection

Electromagnetic damping introduces a practical engineering problem:

> More current → stronger electromagnetic effect → more electrical/thermal stress.

The controller therefore tracks thermal state and prevents the control algorithm from exceeding configured operating limits.

Conceptually:

```text
              Coil Temperature
                     │
          ┌──────────┼──────────┐
          ▼          ▼          ▼
       NORMAL     WARNING     CRITICAL
          │          │          │
          ▼          ▼          ▼
       Normal      Derate      Safe Mode
       Current     Current
```

This demonstrates an important embedded-systems principle:

**The controller must manage the actuator, not merely calculate a command for it.**

---

# 🧵 Lock-Free Telemetry

Telemetry should not compromise the deterministic control loop.

Instead of allowing the web/communication layer to directly interfere with the control task, the architecture uses a **lock-free / low-contention telemetry data exchange**.

```text
              100 Hz Control
                    │
                    │ publish snapshot
                    ▼
             ┌──────────────┐
             │ Telemetry    │
             │ Snapshot     │
             └──────┬───────┘
                    │
                    ▼
                 10 Hz
                    │
                    ▼
             Communication
                    │
                    ▼
              WebSocket
                    │
                    ▼
              Web Dashboard
```

The key architectural rule is:

> **Telemetry can observe the controller. Telemetry must not become the controller's timing bottleneck.**

---

# 📊 Live Telemetry Dashboard

The embedded controller exposes live operating data through a Node.js WebSocket bridge.

The React/Vite frontend can visualize values such as:

* Acceleration
* Filtered acceleration
* Requested damping force
* Coil current
* Temperature
* Controller state
* Safety state
* Fault flags
* Control-loop status

Example dashboard concept:

```text
┌────────────────────────────────────────────────────┐
│          MR SUSPENSION CONTROL MONITOR             │
├────────────────────────────────────────────────────┤
│                                                    │
│   ACCELERATION        DAMPING FORCE                │
│     1.42 m/s²            847 N                     │
│                                                    │
│   COIL CURRENT         TEMPERATURE                 │
│     1.87 A               62.4 °C                   │
│                                                    │
├────────────────────────────────────────────────────┤
│                                                    │
│     ACCELERATION / TIME                             │
│     ╭────╮    ╭──╮       ╭────╮                   │
│    ╱    ╲╱╲──╯  ╰──╲────╯    ╲                   │
│                                                    │
├────────────────────────────────────────────────────┤
│                                                    │
│   CONTROL STATE:       ● ACTIVE                    │
│   SAFETY STATE:        ● NOMINAL                   │
│   CONTROL RATE:        100 Hz                      │
│   TELEMETRY RATE:      10 Hz                       │
│                                                    │
└────────────────────────────────────────────────────┘
```

This provides a useful bridge between **embedded firmware and modern software tooling**.

---

# 🔌 Hardware Architecture

The prototype architecture combines an STM32 control MCU with an Arduino Nano interface/actuator-side component.

```text
                   ┌─────────────────────┐
                   │   Acceleration /    │
                   │   Suspension Sensor │
                   └──────────┬──────────┘
                              │
                              ▼
                     ┌─────────────────┐
                     │      STM32      │
                     │                 │
                     │  FreeRTOS       │
                     │  C++ Controller │
                     └───────┬─────────┘
                             │
                    Control / Command
                             │
                             ▼
                     ┌─────────────────┐
                     │ Arduino Nano /  │
                     │ Actuator-side   │
                     │ Interface       │
                     └────────┬────────┘
                              │
                              ▼
                     ┌─────────────────┐
                     │ Coil Driver     │
                     └────────┬────────┘
                              │
                              ▼
                     ┌─────────────────┐
                     │ Electromagnetic │
                     │ MR Damper       │
                     └─────────────────┘
```

The exact hardware abstraction allows the control software to be developed and tested independently of the physical actuator implementation.

---

# 🧩 Software Architecture

The C++ codebase is organized around modular components rather than one monolithic control function.

```text
src/
├── control/
│   ├── SuspensionController
│   ├── DampingStrategy
│   └── ForceCurrentMapper
│
├── sensors/
│   ├── AccelerationSensor
│   └── SensorFilter
│
├── actuator/
│   ├── CoilController
│   └── ActuatorLimits
│
├── safety/
│   ├── SafetyManager
│   ├── ThermalProtection
│   └── FaultManager
│
├── telemetry/
│   └── TelemetrySnapshot
│
└── platform/
    ├── STM32
    ├── FreeRTOS
    └── Arduino
```

The architecture follows a simple rule:

> **Hardware-specific code stays at the boundary; control logic stays portable.**

That makes it possible to run portions of the controller in a native desktop test environment.

---

# 🧪 Testability

Real-time embedded software should not depend entirely on hardware for validation.

The project therefore includes a **native CMake build** for running unit tests outside the target MCU.

```text
                 ┌──────────────────┐
                 │   C++ Control    │
                 │      Logic       │
                 └────────┬─────────┘
                          │
              ┌───────────┴───────────┐
              │                       │
              ▼                       ▼
       STM32 / FreeRTOS        Native CMake
       Target Build            Test Build
              │                       │
              ▼                       ▼
        Real Hardware            Unit Tests
```

This enables testing of:

* Filtering behavior
* Force/current conversion
* Current saturation
* Safety limits
* Thermal thresholds
* Fault transitions
* Controller state transitions
* Edge cases
* Deterministic control calculations

---

# 🛠️ Technology Stack

### Embedded

* **C++17**
* **STM32**
* **FreeRTOS**
* **Arduino Nano**
* Sensor interfaces
* Electromagnetic actuator control

### Control

* Real-time signal processing
* Acceleration filtering
* Semi-active damping strategy
* Force-to-current mapping
* Current limiting
* Thermal protection
* Fault handling

### Communication

* Lock-free telemetry snapshot
* Serial/interface communication
* WebSocket telemetry bridge

### Backend

* **Node.js**
* WebSocket server

### Frontend

* **React**
* **Vite**
* Real-time telemetry visualization

### Build & Testing

* **CMake**
* Native C++ testing
* Hardware-independent control modules

---

# 📈 Timing Budget

The system is intentionally designed around explicit timing requirements.

```text
CONTROL PERIOD
10 ms ├──────────────────────────────────────────────┤
      │                                              │
      │ Sensor → Filter → Control → Safety → Output │
      │                                              │
      └──────────────────────────────────────────────┘
                         100 Hz


TELEMETRY PERIOD
100 ms ├────────────────────────────────────────────────────────────┤
       │                                                            │
       │              Telemetry Snapshot → WebSocket               │
       │                                                            │
       └────────────────────────────────────────────────────────────┘
                              10 Hz
```

This separation demonstrates the difference between:

**real-time control timing**
and
**human-facing observability timing**.

---

# 🚨 Example Control Cycle

A typical 100 Hz iteration looks like:

```cpp
void controlLoop()
{
    auto acceleration = sensor.readAcceleration();

    auto filteredAcceleration =
        filter.update(acceleration);

    auto desiredForce =
        controller.calculate(filteredAcceleration);

    auto requestedCurrent =
        forceMapper.toCurrent(desiredForce);

    auto safeCurrent =
        safetyManager.limit(requestedCurrent);

    actuator.setCurrent(safeCurrent);

    telemetry.publish({
        acceleration,
        filteredAcceleration,
        desiredForce,
        safeCurrent
    });
}
```

The important part isn't the number of lines.

It's the **separation of responsibilities**.

Each stage can be independently tested, replaced, calibrated, and analyzed.

---

# 🧠 Engineering Decisions

## Why FreeRTOS?

Because the project has multiple activities with different timing requirements.

The suspension controller is time-sensitive, while telemetry and communication are comparatively slow.

FreeRTOS provides a clean mechanism for separating these responsibilities while maintaining predictable scheduling.

---

## Why C++?

The controller benefits from:

* Encapsulation
* Strong type boundaries
* Modular control strategies
* Hardware abstraction
* Testability
* Reusable components

The objective is not to use C++ for abstraction's sake, but to keep the control system **structured without sacrificing embedded performance**.

---

## Why Native CMake Tests?

Hardware-only testing makes debugging expensive and slow.

By isolating control algorithms from hardware interfaces, the same logic can be exercised with synthetic sensor data on a development machine.

That makes it easier to test abnormal conditions before deploying to the MCU.

---

# 🔬 Example Test Scenarios

The test suite is intended to answer questions such as:

```text
✓ What happens when acceleration spikes?

✓ What happens when the requested current exceeds
  the actuator limit?

✓ What happens when temperature crosses the
  warning threshold?

✓ What happens when the sensor becomes invalid?

✓ Does the controller recover from a transient fault?

✓ Does safe mode produce a deterministic output?

✓ Does filtering attenuate high-frequency noise?

✓ Does the force/current mapping remain within
  physical constraints?
```

These are the kinds of questions that matter when moving from a software demo toward an embedded control system.

---

# 🚀 Project Highlights

| Capability         | Implementation                        |
| ------------------ | ------------------------------------- |
| Real-time control  | **100 Hz**                            |
| Telemetry          | **10 Hz**                             |
| RTOS               | **FreeRTOS**                          |
| MCU                | **STM32**                             |
| Language           | **C++17**                             |
| Actuator concept   | **Electromagnetic / MR-fluid damper** |
| Filtering          | Real-time sensor filtering            |
| Safety             | Thermal + current + fault supervision |
| Fault handling     | Safe-mode architecture                |
| Telemetry          | Lock-free snapshot                    |
| Backend            | Node.js + WebSocket                   |
| Frontend           | React + Vite                          |
| Native testing     | CMake                                 |
| Hardware interface | Arduino Nano                          |

---

# 🎓 What This Project Demonstrates

This project goes beyond implementing a control equation.

It demonstrates the complete engineering pipeline:

```text
PHYSICAL SYSTEM
      ↓
SENSOR ACQUISITION
      ↓
DIGITAL SIGNAL PROCESSING
      ↓
CONTROL THEORY
      ↓
ACTUATOR COMMAND
      ↓
SAFETY SUPERVISION
      ↓
REAL-TIME SCHEDULING
      ↓
TELEMETRY
      ↓
WEB VISUALIZATION
      ↓
TESTING & VALIDATION
```

That combination makes the project particularly relevant to:

* Embedded Systems
* Automotive Electronics
* Vehicle Dynamics
* Control Systems
* Robotics
* Mechatronics
* Real-Time Software
* Firmware Engineering
* IoT / Telemetry Systems

---

# 🗺️ Roadmap

### Current Architecture

* [x] STM32 control architecture
* [x] FreeRTOS task model
* [x] C++ control modules
* [x] Sensor acquisition pipeline
* [x] Signal filtering
* [x] Damping-force calculation
* [x] Force → current conversion
* [x] Current limiting
* [x] Thermal safety concept
* [x] Safe-mode architecture
* [x] Telemetry pipeline
* [x] Node.js WebSocket bridge
* [x] React/Vite dashboard
* [x] Native CMake test architecture

### Future Improvements

* [ ] Hardware-in-the-loop testing
* [ ] CAN/CAN-FD vehicle network integration
* [ ] IMU fusion
* [ ] Suspension-position sensing
* [ ] Road-profile estimation
* [ ] Adaptive control strategy
* [ ] Data logging and replay
* [ ] Automated calibration tools
* [ ] Fault-injection testing
* [ ] Hardware performance profiling

---

# 💡 The Bigger Picture

The interesting part of this project isn't simply:

> **"I made an MR damper controller."**

It's:

> **"I designed a complete real-time control pipeline that connects physical sensing, deterministic embedded computation, actuator control, safety supervision, and live observability."**

That is the mindset behind production-grade embedded systems.

---

# 👨‍💻 Skills Demonstrated

**Embedded:**
`STM32` · `FreeRTOS` · `Arduino` · `RTOS Scheduling` · `Hardware Abstraction`

**Programming:**
`C++17` · `CMake` · `Modular Design` · `Unit Testing`

**Control:**
`Signal Filtering` · `Damping Control` · `Force/Current Mapping` · `Feedback Systems`

**Safety:**
`Thermal Protection` · `Current Limiting` · `Fault Detection` · `Safe Mode`

**Systems:**
`Real-Time Architecture` · `Lock-Free Communication` · `Telemetry`

**Full Stack:**
`Node.js` · `WebSocket` · `React` · `Vite`

---

# ⭐ If You're Reviewing This Project

Look beyond the dashboard.

The real engineering challenge is the path:

**sensor → filtering → control → actuator → safety → real-time scheduling → telemetry**

The web UI makes the system visible.

The embedded architecture makes it **real-time**.

The safety layer makes it **responsible**.

And the test architecture makes it **maintainable**.

---

## 📌 One-Line Portfolio Description

> **A 100 Hz C++/FreeRTOS semi-active MR suspension controller on STM32 with electromagnetic damping control, thermal/fault safety, lock-free telemetry, and a real-time React/WebSocket monitoring stack.**
