# System Architecture

## Scope

The system controls one lightweight cabin moving between four floors. The cabin is treated as a single block. There is **no door mechanism** in this project scope.

## Proposed Logical Architecture

```text
+----------------------+       +----------------------+
| Cabin / Hall Buttons | ----> | Request Manager      |
+----------------------+       +----------+-----------+
                                          |
                                          v
                               +----------------------+
                               | Scheduler            |
                               | (LOOK candidate)     |
                               +----------+-----------+
                                          |
                                          v
                               +----------------------+
                               | Main State Controller|
                               +----+------------+----+
                                    |            |
                                    v            v
                         +----------------+  +----------------+
                         | Motor Control  |  | Display Manager|
                         +-------+--------+  +----------------+
                                 |
                                 v
                         +----------------+
                         | H-Bridge Driver|
                         +-------+--------+
                                 |
                                 v
                         +----------------+
                         | DC Geared Motor|
                         +----------------+

+----------------+       +----------------------+
| Floor Sensors  | ----> | Floor / Position Mgr |
+----------------+       +----------+-----------+
                                    |
                                    +----> Main State Controller

+----------------------+       +----------------------+
| Emergency Stop       | ----> | Safety Manager       |
| Upper/Lower Limits   |       +----------+-----------+
+----------------------+                  |
                                          +----> Motor Enable / Safe State
```

The ESP32-S 38-pin board is the planned main controller. The Arduino Uno is available as an alternative or test board, but a two-controller design is not currently required.

## Hardware Boundaries

- ESP32 GPIO pins provide logic/control signals only; they do not power the motor.
- The H-bridge changes motor direction and accepts a PWM speed command.
- The motor uses a suitable external supply. Logic and driver grounds must be referenced correctly as required by the selected driver.
- Driver selection must be based on the selected motor's rated voltage and measured/documented stall current, with suitable thermal and current margin.
- Floor-position sensors report actual arrival. Software time estimates alone must not be treated as reliable floor detection.
- Emergency stop and end-of-travel protection should have a physical safety path, not software alone.

## Suggested Firmware Modules

| Module | Responsibility | Main interface (proposal) |
|---|---|---|
| RequestManager | Store and clear pending requests | addRequest(floor), hasRequest(floor), clearRequest(floor) |
| Scheduler | Select next destination/direction | selectNextFloor(currentFloor, direction, requests) |
| FloorManager | Read sensors and current position | getCurrentFloor(), isFloorDetected(floor) |
| MotorController | Direction, PWM, stop/brake | moveUp(speed), moveDown(speed), stop() |
| SafetyManager | Emergency and travel limits | isEmergencyActive(), isLimitFault() |
| DisplayManager | Present status | updateDisplay(floor, direction, state) |
| ElevatorController | Coordinate all modules | update() |

These signatures are illustrative. Agree on exact interfaces and data types before parallel coding.

## Suggested Controller States

- `UNHOMED` — position not yet verified after startup.
- `IDLE` — waiting for a request.
- `MOVING_UP` — travelling upward.
- `MOVING_DOWN` — travelling downward.
- `ARRIVAL_CHECK` — verifying the target-floor sensor and settling.
- `EMERGENCY` — emergency stop active.
- `FAULT` — sensor, travel-limit, or movement fault requiring defined recovery.

No door-related states are required.

## Scheduling

LOOK is a candidate scheduling algorithm: continue serving requests in the current direction while requests remain ahead, then reverse when there are no more requests in that direction. The team should compare it with FCFS/SCAN using agreed test cases before final approval. The scheduler chooses destinations; it must not directly drive GPIO pins or the motor.

## Design Decisions to Record

For each decision, document the selected approach, alternatives, rationale, assumptions, test evidence, and any limitations.
