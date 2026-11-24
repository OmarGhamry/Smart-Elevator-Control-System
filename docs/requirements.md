# Project Requirements

These requirements reflect the team's current working scope. Items marked **TBD** must be confirmed before implementation or purchasing.

## Confirmed / Current Scope

- A single elevator cabin moves between **four floors**.
- The model uses a lightweight cardboard structure and cabin.
- Approximate overall vertical height is **80 cm**; clarify whether this means total structure height or distance from Floor 1 to Floor 4.
- The cabin is treated as one controlled block. **Door opening, closing, and door obstruction detection are out of scope.**
- The available main controller is an ESP32-S 38-pin board. An Arduino Uno is also available, but using both is not currently required.
- The system uses one reversible DC motor and an H-bridge driver, subject to motor selection and electrical validation.

## Functional Requirements

- **FR-01 — Request input:** Accept floor/cabin requests through the agreed buttons or input interface.
- **FR-02 — Request storage:** Retain multiple pending requests rather than overwriting earlier requests.
- **FR-03 — Floor tracking:** Maintain the current floor using physical floor-position sensors.
- **FR-04 — Destination selection:** Select the next destination using the agreed scheduling policy.
- **FR-05 — Bidirectional movement:** Command the motor to move the cabin up or down through an H-bridge.
- **FR-06 — Arrival and stopping:** Stop at the requested floor when the corresponding sensor confirms arrival.
- **FR-07 — Status display:** Show current floor, direction, and state (for example IDLE, MOVING_UP, MOVING_DOWN, EMERGENCY).
- **FR-08 — Request completion:** Clear a request only after the destination has been reached and confirmed.
- **FR-09 — Idle behavior:** Remain stopped when there are no pending requests.

## Safety Requirements

- **SR-01 — Emergency stop:** Activating the emergency stop must stop motion and place the system in a defined safe state. Prefer a physical, normally-closed stop circuit that interrupts motor-driver power/enable, in addition to firmware handling.
- **SR-02 — Travel limits:** Provide independent upper and lower travel-limit protection so the cabin cannot be driven beyond the shaft.
- **SR-03 — Sensor validation:** Detect implausible sensor combinations or failure to detect an expected floor; do not continue blindly.
- **SR-04 — Startup behavior:** On startup, establish a known/verified cabin position before normal scheduling. The homing approach is TBD.
- **SR-05 — Mechanical retention:** The cabin must be guided and mechanically retained; do not rely on motor holding torque alone to prevent a falling cabin.

## Non-Functional Requirements

- **NFR-01 — Modularity:** Separate request handling, scheduling, sensing, motor actuation, safety, and display.
- **NFR-02 — Testability:** Each module must have repeatable tests before integration.
- **NFR-03 — Responsiveness:** Read buttons and safety inputs while the cabin is moving; avoid long blocking delays.
- **NFR-04 — Reliability:** Invalid or conflicting inputs must not cause undefined movement.
- **NFR-05 — Maintainability:** Document pin assignments, interfaces, wiring, and state transitions.

## Explicitly Out of Scope

- Automatic door opening/closing.
- Door motors, door sensors, and door obstruction logic.
- Multiple elevator cabins (unless the course specification later requires them).

## Open Decisions / TBD

- Exact definition of the 80 cm height and floor spacing.
- Cabin dimensions, empty mass, and target payload.
- Lift mechanism: winch/drum, pulley arrangement, belt, and possible counterweight.
- Final motor model, rated speed, rated torque, stall current, and gearbox durability.
- H-bridge model and current/thermal margin.
- Motor supply voltage/current and power distribution.
- Floor sensor type and exact count/placement; separate end-of-travel protection.
- Button layout: cabin destination buttons and/or hall-call buttons.
- Scheduling algorithm (LOOK is a candidate, not yet approved).
- Display type and exact status information.
- Homing and recovery behavior after power loss or sensor fault.
- Required demo scenarios and course-specific constraints.
