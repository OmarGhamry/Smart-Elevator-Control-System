# Initial Requirements

These are draft requirements for team discussion. Mark each item as confirmed, changed, or out of scope once the project specification is agreed.

## Functional Requirements

- **FR-01 — Request input:** The system shall accept elevator requests from the supported input interfaces.
- **FR-02 — Floor tracking:** The system shall maintain an indication of the elevator's current floor.
- **FR-03 — Destination selection:** The controller shall select a destination from pending valid requests.
- **FR-04 — Movement control:** The system shall command the elevator to move and stop according to the controller's decisions.
- **FR-05 — Door operation:** The system shall control door opening and closing at appropriate stops.
- **FR-06 — Safety handling:** The system shall prevent unsafe door or movement behavior when a configured safety condition is active.
- **FR-07 — Status display:** The system shall provide relevant floor and operating-status information.
- **FR-08 — Request completion:** The system shall update or clear requests when they have been served.

## Safety Requirements

- **SR-01:** An emergency-stop condition shall cause the system to enter its defined safe state.
- **SR-02:** Door obstruction shall be handled according to the agreed safety behavior.
- **SR-03:** The elevator shall not begin normal movement while a door-open condition is active.
- **SR-04:** Sensor inputs used for movement decisions shall be checked before they are acted upon.

## Non-Functional Requirements

- **NFR-01 — Modularity:** The design should separate request handling, control logic, sensing, actuation, and display.
- **NFR-02 — Testability:** Core control behavior should be testable using repeatable scenarios.
- **NFR-03 — Clarity:** Important states and transitions should be documented.
- **NFR-04 — Reliability:** Invalid or conflicting inputs should not cause undefined controller behavior.

## Open Decisions

- Floor count and elevator count.
- Scheduling algorithm (for example, direction-based/SCAN-style scheduling).
- Exact emergency and obstruction behavior.
- Timing constraints.
- Hardware and software platform.
