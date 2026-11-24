# Team Work Plan

Proposed division for a seven-member team. Adjust names and workload after the team confirms skills and availability. Every member should test their own module and coordinate interfaces before implementation.

## Roles and Deliverables

| Member | Role | Responsibilities | Deliverables |
|---|---|---|---|
| 1 | Team Lead & Integration | Track scope, schedule, interfaces, GitHub workflow, integration readiness, and final review | Project plan, interface agreement, integration checklist |
| 2 | Mechanical Design & Construction | Four-floor cardboard/foam-board structure, cabin, guide rails, motor mount, lift line/drum, mechanical retention | Dimensioned sketch, assembled shaft/cabin, mechanical test notes |
| 3 | Motor & Power Electronics | Compare motors, calculate/test lift capability, select H-bridge and supply, implement direction/PWM control | Motor selection record, driver wiring, tested motor-control module |
| 4 | Sensors & Floor Detection | Select and mount floor sensors and end limits, read inputs, identify current floor, handle sensor faults | Sensor wiring, floor-detection module, sensor test report |
| 5 | User Interface & Requests | Button layout, debouncing, request input/storage interface, LCD/LED status display | Input module, request interface, display module |
| 6 | Scheduling & Main Control Logic | Compare FCFS/SCAN/LOOK, implement chosen scheduler and state machine, coordinate module calls | Scheduler module, controller state diagram, logic test cases |
| 7 | Safety & Verification | Emergency-stop circuit coordination, travel-limit tests, fault scenarios, end-to-end verification, demo preparation | Safety test plan, test report, demo checklist |

## Shared Responsibilities

- Member 1 coordinates but does not become the sole integrator or sole tester.
- Members 2 and 3 must coordinate shaft geometry, motor mounting, drum radius, and load testing.
- Members 3 and 4 must agree on motor-driver and sensor electrical interfaces with the ESP32.
- Members 4 and 6 must agree on how floor arrival and current position are represented in code.
- Members 5 and 6 must agree on the request data structure and button-to-floor mapping.
- Member 7 participates in safety design from the beginning, not only at the end.
- Each module owner supplies a small test or demonstration before integration.

## Suggested Milestones

### M1 — Requirements and Interface Freeze
- Confirm course requirements and project scope.
- Confirm floor numbering and the 80 cm measurement.
- Agree on button layout, sensor strategy, and initial scheduler candidate.
- Agree on module interfaces and pin-allocation ownership.

### M2 — Mechanical and Electrical Proof of Concept
- Build a small secured lift test rig.
- Measure cabin mass and estimate payload.
- Test candidate motor, drum, and lift line.
- Verify H-bridge current/voltage compatibility and power supply.
- Test floor sensors and emergency/travel-limit inputs independently.

### M3 — Independent Module Completion
- Each owner completes and tests their module.
- Publish wiring, pin assignments, interfaces, and test evidence.
- Open Pull Requests targeting `develop`.

### M4 — Integration
- Integrate reviewed changes on `develop`: request input, scheduler, floor detection, motor control, safety, and display.
- Run controlled tests with the cabin secured and a mechanical catch in place.
- Fix integration issues before full-height operation.

### M5 — Verification and Demonstration
- Test all four floors, simultaneous requests, both directions, idle behavior, emergency stop, and travel limits.
- Record expected versus actual behavior.
- Prepare final explanation, diagrams, and demonstration.
- The repository owner promotes a verified stable milestone from `develop` to `main`.

## GitHub Workflow

- `main` is the protected stable branch; only `@OmarGhamry` may push to or merge into it.
- `develop` is the team's shared integration branch and the target for all contributor Pull Requests.
- Create one task branch per feature or task from `develop` (for example, `feature/floor-detection`).
- Contributors must not push directly to `develop` or `main`.
- Open Pull Requests from task branches into `develop`, not `main`.
- The owner/integration lead reviews and merges approved changes into `develop`.
- Only the owner promotes stable, tested work from `develop` into `main`.
- Do not commit credentials, personal tokens, or unrelated generated files.

## Decisions to Record at the First Team Meeting

- Exact meaning of the 80 cm height and floor spacing.
- Cabin dimensions, empty mass, and target payload.
- Lift mechanism and safe mechanical retention.
- Motor model, H-bridge, and supply.
- Floor sensors and independent travel-limit arrangement.
- Button layout and request behavior.
- Scheduling algorithm (LOOK is a candidate, not yet final).
- Display choice and firmware toolchain.
- Names assigned to each role and milestone dates.
