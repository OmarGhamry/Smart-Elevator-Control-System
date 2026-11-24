# Tests

Record each test's setup, inputs, expected result, actual result, and pass/fail status. Test modules independently before full integration.

## Request and Scheduling Tests

- No pending requests: elevator remains stopped in IDLE.
- A request is made for the current floor.
- A request is made above the current floor.
- A request is made below the current floor.
- Multiple requests are queued before movement starts.
- Requests exist both ahead of and behind the current direction.
- Scheduler serves requests in the agreed order (LOOK is a candidate until approved).
- A request is not cleared before arrival is physically confirmed.

## Movement and Position Tests

- Motor moves upward when commanded.
- Motor moves downward when commanded.
- Stop command stops the motor as expected.
- Each of the four floor sensors is detected correctly.
- Cabin stops at each floor without overshooting.
- Startup position is established using the agreed homing procedure.
- Loss or implausible combination of floor sensor signals causes a safe fault response.

## Safety Tests

- Emergency stop interrupts motion and prevents restart until safely reset.
- Upper travel limit prevents further upward travel.
- Lower travel limit prevents further downward travel.
- A disconnected/stuck sensor is handled according to the fault policy.
- Power interruption and restart do not cause uncontrolled movement.

## User Interface Tests

- Each button maps to the correct floor/request.
- Multiple button presses are retained.
- Display shows correct floor, direction, and state.
- Display returns to IDLE when all requests are served.

## Mechanical / End-to-End Tests

- Cabin travels through the full shaft without binding.
- Guide rails prevent excessive swinging.
- Lift line/belt remains attached and does not slip.
- Test with empty cabin, then increase payload gradually within a documented safe test limit.
- Repeat full travel cycles and inspect motor, driver, wiring, and gearbox temperature.

Never test by placing hands beneath a suspended cabin. Secure the shaft and provide a mechanical catch/support during early load tests.
