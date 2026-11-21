# System Architecture

## Proposed Logical Modules

```text
+----------------------+       +----------------------+
| Floor / Cabin Inputs | ----> | Request Manager      |
+----------------------+       +----------+-----------+
                                          |
                                          v
                               +----------------------+
                               | Elevator Controller  |
                               +----+------------+----+
                                    |            |
                         +----------+            +-----------+
                         v                                   v
                +----------------+                  +----------------+
                | Motor Control  |                  | Door Control   |
                +----------------+                  +----------------+
                         ^                                   ^
                         |                                   |
                +----------------+                  +----------------+
                | Floor Sensors  |                  | Safety Inputs  |
                +----------------+                  +----------------+
                                    |
                                    v
                           +------------------+
                           | Status Display   |
                           +------------------+
```

This is a logical starting point, not a final circuit or implementation diagram. The team should revise it to match the selected hardware and actual signal flow.

## Suggested Controller States

- `IDLE` — waiting for a request.
- `MOVING_UP` — travelling upward.
- `MOVING_DOWN` — travelling downward.
- `STOPPING` — reaching and settling at a destination.
- `DOOR_OPEN` — doors are open.
- `DOOR_CLOSING` — doors are closing.
- `EMERGENCY` — emergency handling state.

The final state machine must define valid transitions, priorities, and recovery behavior.

## Design Decisions to Record

For each major decision, document:
- The selected approach.
- Alternatives considered.
- Why the team selected it.
- Any assumptions or limitations.
