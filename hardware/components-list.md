# Hardware Plan and Preliminary Bill of Materials

This is a planning checklist, **not an approved purchase order**. Confirm dimensions, cabin mass, payload, lift geometry, motor current, and driver ratings before buying parts.

## Available Boards

| Component | Status | Notes |
|---|---|---|
| ESP32-S, 38-pin | Available | Planned main controller |
| Arduino Uno | Available | Backup/test option; no need to use both unless a clear reason emerges |

## Main Hardware

| Component | Quantity (initial estimate) | Purpose | Selection notes |
|---|---:|---|---|
| DC geared motor | 1 | Raise/lower cabin | Compare TT 3–6V and metal-geared alternatives after load/torque test |
| H-bridge motor driver | 1 | Reverse motor direction and control speed with PWM | Must support motor voltage and stall current with thermal margin |
| Regulated motor power supply | 1 | Power motor/driver | Choose voltage/current from final motor datasheet and measured load |
| Floor-position sensors | 4 (provisional) | Detect arrival at each floor | Limit switches or Hall sensors; placement and logic TBD |
| Upper/lower travel-limit switches | 2 (recommended) | Independent end-of-travel protection | Should stop motion in the unsafe direction |
| Emergency-stop switch | 1 | Stop motion during emergency | Prefer normally-closed physical circuit interrupting motor enable/power |
| Push buttons | TBD | Cabin destination and/or hall calls | Quantity depends on agreed interface |
| LCD 16x2 with I2C (optional) | 1 | Show floor, direction, and state | Can be replaced by LEDs/serial output if budget or pins are limited |
| Wires, connectors, breadboard/perfboard | As needed | Wiring and assembly | Use secure connections for motor current |
| Mechanical guide rails | 2 | Guide cabin vertically | Smooth, parallel rails reduce binding |
| Lift line/belt and drum/pulley | 1 set | Transfer motor motion to cabin | Drum radius affects force and speed |
| Cardboard / foam board / supports | As needed | Four-floor model and cabin | Reinforce motor mount and shaft structure |
| Mechanical catch/support | 1 | Prevent cabin drop during tests | Important during early load testing |

## Motor Candidates

### Candidate A — TT 3–6V DC Geared Motor

- Low-cost, compact, and geared; suitable for initial experiments with a very light model.
- Usually uses a plastic gearbox and commonly has no encoder.
- Exact speed, torque, and stall current vary by supplier and version.
- Do not assume a published stall torque is a safe continuous operating torque.

### Candidate B — Metal-Geared DC Motor (for example, a JGB37-520 variant)

- Larger and generally more mechanically robust.
- Some versions include an encoder; verify the exact listing.
- Available gear ratios and output speeds vary significantly.
- May require a different driver and higher-current supply.

### Motor Selection Checklist

Before approval, record:
- Exact model / supplier datasheet.
- Rated voltage and rated output speed.
- Rated torque (not only stall torque).
- Stall current.
- Gearbox type and gear ratio.
- Drum radius and lift arrangement.
- Measured empty cabin mass and target payload.
- Test result lifting the intended load repeatedly without overheating or slipping.

## H-Bridge Selection Checklist

DRV8833 and TB6612FNG are possible candidates for small DC motors, **not automatic selections**. Check:
- Motor supply voltage range.
- Continuous current per channel.
- Peak/stall current capability and duration.
- Thermal performance of the actual breakout board.
- Logic input compatibility with ESP32 3.3V signals.
- Required PWM frequency and standby/enable behavior.

Avoid selecting a driver based only on the current printed in a product title. The final motor's stall current and the board's real thermal capability matter.

## Power and Wiring Notes

- Never connect the motor directly to an ESP32 GPIO pin.
- Use a suitable external motor supply; do not power the motor from the ESP32 3.3V pin.
- Connect grounds as required by the chosen driver and circuit design.
- Keep motor-current wiring separate from sensitive sensor wiring where practical.
- Add appropriate decoupling and follow the driver's datasheet.
- The emergency stop should provide a physical safe stop path; firmware alone is not sufficient.

## Purchase Gate

Do not place the final order until the team has:
1. Measured the cabin and estimated the maximum payload.
2. Chosen the mechanical lift geometry and drum radius.
3. Tested the candidate motor with a secured test rig.
4. Verified motor current and driver ratings.
5. Agreed on sensor count, button layout, and power requirements.
