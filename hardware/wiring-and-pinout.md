# Proteus Wiring and Pin Map

This document records the wiring tested in the current Arduino Uno simulation. It is not a substitute for the datasheets and safety review required for a physical prototype.

## Arduino Uno Mapping

| Arduino pin | Signal / component | Verified behavior |
|---|---|---|
| A0 | Emergency signal | HIGH normally; LOW when the normally-closed button is pressed |
| A1 | Speaker | Use `tone()` / `noTone()` for a clean tone |
| A4 | PCF8574 SDA | I2C data |
| A5 | PCF8574 SCL | I2C clock |
| D2 | 74HC595 DS | Serial data |
| D3 | 74HC595 SH_CP | Shift clock |
| D4 | 74HC595 ST_CP | Latch clock |
| D5 | L298 IN1 | Motor direction |
| D6 | L298 IN2 | Motor direction |
| D7 | Available | Not assigned |
| D8 | Available | Not assigned |
| D9 | L298 ENA through AND gate | PWM enable; default test PWM 150 |
| D10 | IR sensor, Floor 1 | HIGH on detection |
| D11 | IR sensor, Floor 2 | HIGH on detection |
| D12 | IR sensor, Floor 3 | HIGH on detection |
| D13 | IR sensor, Floor 4 | HIGH on detection |

Avoid using D0/D1 for application I/O while the Virtual Terminal is used for Serial debugging.

## PCF8574

- Address: `0x20` (A0, A1, A2 tied to GND).
- SDA (pin 15) -> Arduino A4.
- SCL (pin 14) -> Arduino A5.
- INT (pin 13) is not used in the current polling design.
- VDD/VSS are the hidden supply pins in the Proteus symbol; verify the power nets are connected.

| Port | Button |
|---|---|
| P0 | External call — Floor 1 |
| P1 | External call — Floor 2 |
| P2 | External call — Floor 3 |
| P3 | External call — Floor 4 |
| P4 | Internal destination — Floor 1 |
| P5 | Internal destination — Floor 2 |
| P6 | Internal destination — Floor 3 |
| P7 | Internal destination — Floor 4 |

Each button connects its PCF8574 port pin to GND. Write `0xFF` before reading the quasi-bidirectional inputs. A pressed button reads LOW.

## 74HC595 and Common-Anode Display

| 74HC595 pin | Connection |
|---|---|
| 16 VCC | +5V |
| 8 GND | GND |
| 14 DS | Arduino D2 |
| 11 SH_CP | Arduino D3 |
| 12 ST_CP | Arduino D4 |
| 13 OE | GND (enabled) |
| 10 MR | +5V (inactive reset) |
| Q0..Q7 | Segments A, B, C, D, E, F, G, DP respectively |

The display is common-anode: common pins connect to +5V and each segment is active LOW. Use an individual series current-limiting resistor for every segment used (including DP if connected). The simulation uses 220 ohms per segment.

## Motor Driver

- L298 module: +5V logic supply, +12V motor supply in the tested Proteus setup, and common GND.
- IN1 -> D5; IN2 -> D6; ENA -> output of the emergency AND gate.
- D9 PWM is one AND-gate input; the normal/emergency signal is the other input.
- OUT1/OUT2 -> motor terminals.
- The two small terminals beneath OUT2 on the specific TheEngineeringProjects Proteus module were connected to GND to make the simulation model operate. This is specific to that library model; do not generalize it to a physical L298 module without checking its pinout.

The tested PWM value is 150. It is a simulation starting point, not a calibrated physical speed.

## Emergency Circuit

- +5V -> normally-closed emergency button -> emergency signal node.
- Emergency signal node -> 10kΩ pull-down resistor -> GND.
- Emergency signal node -> Arduino A0 and one AND-gate input.
- Arduino D9 PWM -> other AND-gate input.
- AND-gate output -> L298 ENA.

Normal state reads HIGH and allows PWM through. Pressing the NC switch opens the circuit, the signal becomes LOW, and the AND gate forces ENA LOW. Firmware must also latch emergency; releasing the button must not automatically resume motion.

This is an educational simulation circuit, not a certified safety circuit.

## IR Sensor Model

- Four sensors, one per floor.
- VCC -> +5V; GND -> GND.
- OUT -> D10, D11, D12, D13 for Floors 1..4.
- Each sensor model uses `InfraredSensorsTEP.HEX` in its Proteus Program File.
- TEST PIN is driven by a LOGICSTATE during simulation.
- Verified polarity: TEST PIN LOW -> OUT LOW; TEST PIN HIGH -> OUT HIGH.
- The model showed a delayed LOW transition when detection was removed; this was observed in Proteus and must not be assumed to represent real sensor timing.

## Speaker and Serial Debugging

- Speaker positive/first terminal -> A1; other terminal -> GND in the tested setup.
- Use `tone(A1, frequency)` and `noTone(A1)`; a constant HIGH did not produce the intended sound with the selected Speaker component.
- Optional Virtual Terminal: Arduino TX/D1 -> terminal RXD, 9600 baud. The terminal is useful for debugging and can be removed from the final presentation if Serial output is no longer needed.
