# System Overview

## Purpose

Build a four-floor smart elevator model for the Computer Organization course. The project demonstrates button input handling, request management, direction-based scheduling, floor sensing, bidirectional motor control, display output, and emergency handling.

## Current Validated Simulation

- Four floors.
- Arduino Uno R3 as the Proteus simulation controller.
- Four IR obstacle-sensor models, one at each floor.
- Eight request buttons: four external hall calls and four internal cabin destinations.
- PCF8574 I2C expander at address 0x20.
- L298 H-bridge Proteus module and a reversible DC motor.
- 74HC595 driving a common-anode 7-segment display.
- Speaker on A1 for audible alerts.
- Normally-closed emergency button read on A0; the emergency signal is HIGH normally and LOW when pressed.
- Emergency hardware gate forces motor enable low when the emergency switch is pressed.
- No automatic door mechanism.

## Current Pin Map

See [../hardware/wiring-and-pinout.md](../hardware/wiring-and-pinout.md) for the complete mapping. Arduino pins currently reserved include A0/A1/A4/A5 and D2-D6/D9-D13.

## Operating Cycle (Target Firmware)

1. Initialize the I/O devices and safety signal.
2. Start in `POSITION_UNKNOWN` until a valid floor sensor establishes position.
3. Scan the eight buttons and capture new presses without blocking.
4. Store internal and external requests independently.
5. Check the emergency input continuously and latch emergency when activated.
6. When position is known and no emergency is active, ask the scheduler for the next destination.
7. Drive the motor in the selected direction.
8. Stop when the target-floor sensor confirms arrival.
9. Update the current floor, display, and request state.
10. Continue servicing requests until none remain, then stay in `IDLE`.

## Important Simulation Notes

- The tested IR model output is HIGH when its TEST PIN is HIGH (simulated detection) and LOW otherwise.
- The tested PCF8574 address is 0x20 with A0/A1/A2 grounded. Buttons are active LOW on P0-P7.
- The common-anode 7-segment segments are active LOW and are driven through individual current-limiting resistors.
- The speaker produces a cleaner tone with `tone()` / `noTone()` than with a constant HIGH level.
- The current motor PWM test value is 150; it is a simulation starting point, not a calibrated physical speed.

## Limitations and Next Validation

Proteus tests verify the configured simulation models and firmware logic, not the real sensor range, motor inertia, mechanical alignment, load capacity, braking, current limits, or safety performance. The physical prototype must be tested separately and must not be operated as a passenger elevator.
