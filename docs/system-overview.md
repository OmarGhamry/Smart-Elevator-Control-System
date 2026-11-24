# System Overview

## Purpose

Build a small, working smart elevator model for the Computer Organization course. The model demonstrates request handling, scheduling, position sensing, bidirectional motor control, and safety handling.

## Current Scope

- One cabin serving **four floors**.
- Cardboard structure and lightweight cardboard cabin.
- Approximate vertical height: **80 cm** (the team must confirm whether this is total structure height or Floor 1-to-Floor 4 travel).
- ESP32-S 38-pin as the planned main controller.
- One reversible DC motor with a suitable gearbox, driven through an H-bridge.
- Physical floor-position sensing.
- Request buttons and a status display.
- Emergency stop and upper/lower travel-limit protection.
- **No door mechanism:** the cabin is treated as a single block; door opening/closing is not modeled.

## High-Level Operating Cycle

1. Initialize inputs, outputs, display, and safety signals.
2. Establish or verify the cabin's current position (homing strategy TBD).
3. Read and validate new floor requests.
4. Store pending requests.
5. Check emergency and travel-limit conditions.
6. Scheduler selects the next destination.
7. Motor controller moves the cabin in the selected direction.
8. Floor sensor confirms arrival; motor is stopped.
9. Update the current floor and display; clear the served request.
10. Repeat until no requests remain, then remain idle.

## Current Hardware Candidates (Not Yet Approved)

- ESP32-S 38-pin development board.
- TT 3–6V DC geared motor as a low-cost candidate for a very light model.
- A higher-torque metal-geared DC motor (for example, a JGB37-520 variant) as an alternative if the load and test results require it.
- H-bridge such as DRV8833 or TB6612FNG only if its voltage/current ratings suit the selected motor.
- Limit switches or Hall sensors for floor detection.
- External motor power supply selected from motor specifications.

Do not purchase based only on nominal voltage or advertised stall torque. First estimate cabin plus payload mass, drum radius, friction, required speed, motor current, and driver thermal/current limits.

## Assumptions to Confirm

- Exact floor spacing and what the 80 cm measurement represents.
- Cabin dimensions and maximum payload.
- Mechanical lift method and safe cabin guidance/retention.
- Motor, gearbox, driver, supply, and sensor models.
- Button arrangement and scheduling policy.
- Homing and fault recovery behavior.
- Final demonstration and course requirements.
