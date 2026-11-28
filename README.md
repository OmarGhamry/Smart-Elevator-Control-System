# Smart Elevator Control System

An academic **Computer Organization** project to design and simulate a four-floor smart elevator, then use the validated design as the basis for a physical prototype.

## Current Prototype

| Item | Validated design / current decision |
|---|---|
| Floors | 4 |
| Main simulation controller | Arduino Uno R3 in Proteus |
| Cabin movement | One reversible DC motor using an H-bridge |
| Motor driver model | L298 Proteus module (TheEngineeringProjects library) |
| Motor command | Direction pins plus PWM; current test speed is PWM 150 |
| Floor detection | 4 IR obstacle-sensor models, one per floor |
| IR output logic | HIGH when the simulated sensor detects the cabin; LOW otherwise |
| User inputs | 4 hall-call buttons + 4 cabin-destination buttons |
| Button expander | PCF8574 at I2C address 0x20 |
| Display | Common-anode 7-segment display driven by 74HC595 |
| Audio | Speaker connected to A1; tone generated with tone()/noTone() |
| Emergency | Normally-closed button, active-low signal to A0, plus a hardware motor-enable gate |
| Doors | Out of scope; no automatic door mechanism |

The current pin map and component behavior are documented in [hardware/wiring-and-pinout.md](hardware/wiring-and-pinout.md). Proteus tests validate the simulation model only; physical motor, sensor, power, braking, and safety behavior must be tested separately before any real-world use.

## Main Goals

- Accept and store cabin and hall requests independently.
- Handle simultaneous button presses.
- Schedule pending requests using a direction-based collective policy (LOOK-like).
- Determine the current floor from the four IR sensors.
- Drive the motor up/down and stop on a detected floor.
- Display the current floor using the 7-segment display.
- Provide audible arrival and fault/emergency alerts.
- Latch an emergency stop until a deliberate manual reset is accepted after the emergency switch is released and the cabin position is confirmed.

## Firmware Modules

| Module | Responsibility |
|---|---|
| Input Manager | Read PCF8574 buttons and emergency signal |
| Request Manager | Store and clear internal/external requests |
| Position Manager | Interpret the four floor sensors and track position validity |
| Scheduling Manager | Select the next requested floor |
| Motor Controller | Control direction and PWM through the H-bridge |
| Safety Manager | Latch emergency and inhibit motor operation |
| Display Manager | Update the 7-segment display |
| Audio Manager | Generate speaker alert patterns |
| Elevator Controller | Coordinate the state machine and modules |

## Controller States

- `POSITION_UNKNOWN` — startup position has not yet been verified.
- `IDLE` — positioned and waiting for a request.
- `MOVING_UP` — travelling upward.
- `MOVING_DOWN` — travelling downward.
- `EMERGENCY` — emergency stop latched; manual recovery required.

## Repository Structure

```text
docs/       Scope, requirements, architecture, and team plan
hardware/   Validated Proteus component list and pin/wiring reference
src/        Arduino firmware
simulation/ Proteus simulation files (to be added)
tests/      Test cases and verification notes
```

## Development Status

- [x] Individual Proteus tests: motor direction/PWM, emergency hardware stop, four IR sensors, eight PCF8574 buttons, 74HC595 + common-anode display, and speaker output.
- [x] Pin assignments recorded.
- [x] Firmware foundation, Input Manager, Position Manager, Request Manager, LOOK-style Scheduling Manager, Safety Manager, and direction-neutral Motor Controller implemented. Proteus tests passed for emergency latch/reset, both motor rotation directions, motor stop, and emergency stop during motor rotation.
- [ ] Integrate Scheduler, Position Manager, Safety Manager, and Motor Controller; then run integrated state-machine and scheduler tests.
- [ ] Full-system Proteus scenarios.
- [ ] Physical prototype validation and safety review.

## Team Git Workflow

- `main` is the stable/release branch. Only the repository owner, `@OmarGhamry`, may push to or merge into it.
- `develop` is the team's integration branch and the target for all team Pull Requests.
- Contributors create task branches from `develop`, such as `feature/floor-detection` or `fix/sensor-debounce`.
- Open Pull Requests from task branches into `develop`; never target `main` with a team PR.
- Stable milestones are promoted from `develop` to `main` by the owner.

See [CONTRIBUTING.md](CONTRIBUTING.md), [docs/github-setup.md](docs/github-setup.md), and [docs/team-work-plan.md](docs/team-work-plan.md).

## Academic Context

Developed as a team project for the Computer Organization course.
