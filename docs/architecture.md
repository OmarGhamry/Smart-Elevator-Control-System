# System Architecture

## Scope and Prototype Boundary

The current implementation target is a **four-floor Arduino Uno R3 simulation in Proteus**. The physical prototype is a later stage and must be revalidated against real component datasheets and measured mechanical loads. There is no automatic door mechanism.

## Validated Hardware Interfaces

| Function | Interface |
|---|---|
| Hall/cabin buttons | PCF8574, I2C address 0x20 |
| Floor detection | Four IR sensors, one per floor; output HIGH on detection |
| Motor | L298 H-bridge module; IN1/IN2 set direction, ENA receives PWM through the emergency AND gate |
| Emergency | NC switch signal on A0; HIGH in normal condition, LOW when pressed |
| Display | Common-anode 7-segment through 74HC595; segments are active LOW |
| Audio | Speaker on A1, driven with tone()/noTone() |

See [../hardware/wiring-and-pinout.md](../hardware/wiring-and-pinout.md) for the full pin assignment.

## Firmware Modules

| Module | Responsibility |
|---|---|
| InputManager | Read the PCF8574 and emergency input; debounce and detect new press events |
| RequestManager | Maintain separate internal and external request arrays |
| PositionManager | Read floor sensors, identify arrival, and flag unknown/invalid position |
| SchedulingManager | Choose a target using direction-based collective scheduling |
| MotorController | Set motor direction, PWM, and stop |
| SafetyManager | Latch emergency and block motor commands until explicit recovery |
| DisplayManager | Display floor/status using the 74HC595 |
| AudioManager | Play arrival and alert patterns on the speaker |
| ElevatorController | Coordinate modules and state transitions |

## State Model

- `POSITION_UNKNOWN`: startup position has not been verified.
- `IDLE`: cabin is at a known floor and has no active movement.
- `MOVING_UP`: cabin is moving upward.
- `MOVING_DOWN`: cabin is moving downward.
- `EMERGENCY`: emergency has been latched; releasing the NC switch must not resume motion.

A sensor conflict, missed floor detection, or other position-integrity problem must not be treated as a valid floor arrival. The safe response is to stop and keep the position unknown until a recovery procedure is defined.

## Request Model and Scheduling

Requests are stored separately:

```cpp
bool internalRequests[4] = {false};
bool externalRequests[4] = {false};
```

The scheduler follows a LOOK-like policy: continue serving eligible requests in the current direction; reverse only when there are no remaining requests ahead. The scheduler selects destinations only—it does not write motor GPIO pins.

A button press is an event, not a request that should disappear when the button is released. The input layer must debounce and detect press transitions, and the request layer must preserve pending requests until they are served or explicitly cleared by a defined recovery rule.

## Non-blocking Runtime

The integrated firmware should use a cooperative, non-blocking loop based on `millis()` rather than long `delay()` calls. This allows button scanning, emergency monitoring, position sensing, display refresh, and audio timing to continue while the cabin is moving.

## Safety Boundary

The Proteus emergency path currently gates the L298 enable signal in hardware and reports the NC switch state to A0. Firmware must also latch the emergency state. This educational simulation is **not a certified elevator safety system**. A physical build requires independent engineered braking, travel-limit protection, secure mechanical retention, correctly rated power electronics, and a reviewed emergency circuit.

## Implementation Order

1. Pin definitions and shared state.
2. Input Manager and button debouncing.
3. Position Manager.
4. Request Manager.
5. Motor Controller.
6. Scheduling Manager.
7. Safety Manager and recovery behavior.
8. Display and audio managers.
9. Main controller integration and scenario testing.
