# MR-Fluid Semi-Active Suspension Control System

## Real-Time Embedded Suspension Control, Telemetry and Visualization Platform

A real-time semi-active suspension control system designed around a **Magnetorheological (MR) fluid damper / controllable electromagnetic suspension actuator**.

The project combines:

- Mechanical suspension dynamics
- MR-fluid damping concept
- Force-to-current control
- C++ real-time control software
- STM32 embedded control
- FreeRTOS task scheduling
- Sensor acquisition
- Coil-current control
- Thermal protection
- Safe-mode handling
- 100 Hz control loop
- 10 Hz telemetry
- Lock-free telemetry buffering
- Node.js WebSocket bridge
- React/Vite real-time dashboard
- Unit testing
- CMake build system

The system is intended as a prototype/reference architecture for an intelligent semi-active automotive suspension controller.

---

# 1. Project Overview

The objective of this project is to demonstrate how suspension acceleration measurements can be processed in real time and converted into an appropriate electromagnetic actuator current.

The basic control chain is:

```text
Suspension / Vehicle Dynamics
          |
          v
   Acceleration Sensor
          |
          v
     Sensor Reading
          |
          v
      Signal Filter
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
   Thermal / Safety Check
          |
          v
   MR Damper Coil Driver
          |
          v
      Suspension
