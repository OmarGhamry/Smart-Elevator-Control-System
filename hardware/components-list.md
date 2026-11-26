# Hardware Components — Proteus Prototype

This list records the components currently used and individually tested in the Proteus simulation. It is **not** a final bill of materials for the physical prototype.

## Tested Components

| Component | Qty | Role | Test / configuration |
|---|---:|---|---|
| Arduino Uno R3 | 1 | Main simulation controller | Proteus firmware target |
| L298 module (TheEngineeringProjects model) | 1 | H-bridge motor driver | Forward/reverse and stop tested; custom model's two small terminals beneath OUT2 were connected to GND as required by this library model |
| DC motor | 1 | Cabin movement | Operates in both directions in simulation |
| IR obstacle sensor (TheEngineeringProjects model) | 4 | One floor sensor per floor | Uses `InfraredSensorsTEP.HEX`; TEST PIN HIGH produces OUT HIGH |
| PCF8574 | 1 | Eight button inputs | I2C address 0x20; A0/A1/A2 grounded |
| Push buttons | 8 | Four external + four internal requests | Each PCF8574 input is active LOW when pressed |
| 74HC595 | 1 | Serial-to-parallel display driver | DS=D2, SH_CP=D3, ST_CP=D4, OE=GND, MR=+5V |
| Common-anode 7-segment | 1 | Floor display | Q0-Q7 map to A-G,DP; active LOW; one series resistor per segment |
| Resistors | 8 | 7-segment current limiting | One per A-G and DP segment; 220 ohms used in the current simulation wiring |
| Emergency NC push button | 1 | Emergency input | A0 reads HIGH normally and LOW when pressed |
| AND gate | 1 | Hardware emergency motor-enable gate | Combines PWM enable with emergency-normal signal |
| Speaker | 1 | Audible alerts | Connected to A1; use tone()/noTone() |
| Virtual Terminal | 1 | Serial debugging | RXD connected to Arduino TX/D1; optional during integrated debugging |

## Arduino Uno Pin Assignment

| Arduino pin | Connected component | Notes |
|---|---|---|
| A0 | Emergency signal | NC switch; HIGH normal, LOW emergency |
| A1 | Speaker | Tone output |
| A4 | PCF8574 SDA | I2C data |
| A5 | PCF8574 SCL | I2C clock |
| D2 | 74HC595 DS | Serial data |
| D3 | 74HC595 SH_CP | Shift clock |
| D4 | 74HC595 ST_CP | Latch clock |
| D5 | L298 IN1 | Motor direction |
| D6 | L298 IN2 | Motor direction |
| D7 | Available | Unassigned in the current map |
| D8 | Available | Unassigned in the current map |
| D9 | L298 ENA via AND gate | PWM motor enable |
| D10 | IR sensor — Floor 1 | Detection active HIGH |
| D11 | IR sensor — Floor 2 | Detection active HIGH |
| D12 | IR sensor — Floor 3 | Detection active HIGH |
| D13 | IR sensor — Floor 4 | Detection active HIGH |

## PCF8574 Button Mapping

| PCF8574 pin | Button |
|---|---|
| P0 | External call — Floor 1 |
| P1 | External call — Floor 2 |
| P2 | External call — Floor 3 |
| P3 | External call — Floor 4 |
| P4 | Internal destination — Floor 1 |
| P5 | Internal destination — Floor 2 |
| P6 | Internal destination — Floor 3 |
| P7 | Internal destination — Floor 4 |

Each button connects its PCF8574 port pin to GND. Write 0xFF to the PCF8574 before reading its quasi-bidirectional pins.

## Power and Ground Notes

- Confirm the logic and motor supply ratings against the exact selected driver and motor before building physical hardware.
- Use a common reference ground where required by the driver and control circuit.
- The Proteus L298 library model has model-specific pins; do not assume every hidden/extra pin maps identically to a commercial L298N breakout.
- The physical circuit requires current limiting, decoupling, correct wire sizing, and independent safety design.

## Not Yet Finalized for Physical Hardware

Motor model and rated/stall current, motor supply, mechanical load, braking, floor-sensor mounting offsets, travel-limit protection, and final physical safety design remain to be validated. The present simulation must not be treated as a certified elevator controller.
