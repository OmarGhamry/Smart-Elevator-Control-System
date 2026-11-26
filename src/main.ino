/*
  Smart Elevator Control System
  Initial firmware foundation — Arduino Uno R3 / Proteus simulation.

  This file currently contains the validated pin map and shared state only.
  Module behavior will be added incrementally and tested after each step.
*/

#include <Arduino.h>
#include <Wire.h>

// -----------------------------------------------------------------------------
// System configuration
// -----------------------------------------------------------------------------

constexpr uint8_t FLOOR_COUNT = 4;
constexpr uint8_t PCF8574_ADDRESS = 0x20;
constexpr unsigned long BUTTON_SCAN_INTERVAL_MS = 20;
constexpr unsigned long BUTTON_DEBOUNCE_MS = 40;

// -----------------------------------------------------------------------------
// Arduino Uno pin mapping
// -----------------------------------------------------------------------------

// Safety and audio
constexpr uint8_t EMERGENCY_PIN = A0;  // NC switch: HIGH = normal, LOW = pressed
constexpr uint8_t SPEAKER_PIN   = A1;  // Speaker driven with tone()/noTone()

// I2C: PCF8574
constexpr uint8_t I2C_SDA_PIN = A4;
constexpr uint8_t I2C_SCL_PIN = A5;

// 74HC595 -> common-anode 7-segment display
constexpr uint8_t DISPLAY_DATA_PIN  = 2;  // DS
constexpr uint8_t DISPLAY_CLOCK_PIN = 3;  // SH_CP
constexpr uint8_t DISPLAY_LATCH_PIN = 4;  // ST_CP

// L298 motor driver
constexpr uint8_t MOTOR_IN1_PIN = 5;
constexpr uint8_t MOTOR_IN2_PIN = 6;
constexpr uint8_t MOTOR_ENA_PIN = 9;      // PWM through the hardware emergency AND gate
constexpr uint8_t DEFAULT_MOTOR_PWM = 150;

// One IR sensor per floor; tested output is HIGH on detection.
const uint8_t FLOOR_SENSOR_PINS[FLOOR_COUNT] = {10, 11, 12, 13};
constexpr uint8_t FLOOR_SENSOR_ACTIVE_LEVEL = HIGH;

// -----------------------------------------------------------------------------
// PCF8574 button mapping (buttons connect the port pin to GND)
// -----------------------------------------------------------------------------

enum ButtonBit : uint8_t {
  EXTERNAL_FLOOR_1 = 0,  // P0
  EXTERNAL_FLOOR_2 = 1,  // P1
  EXTERNAL_FLOOR_3 = 2,  // P2
  EXTERNAL_FLOOR_4 = 3,  // P3
  INTERNAL_FLOOR_1 = 4,  // P4
  INTERNAL_FLOOR_2 = 5,  // P5
  INTERNAL_FLOOR_3 = 6,  // P6
  INTERNAL_FLOOR_4 = 7   // P7
};

// -----------------------------------------------------------------------------
// Shared application state
// -----------------------------------------------------------------------------

enum class ElevatorState : uint8_t {
  POSITION_UNKNOWN,
  IDLE,
  MOVING_UP,
  MOVING_DOWN,
  EMERGENCY
};

enum class Direction : int8_t {
  DOWN = -1,
  STOPPED = 0,
  UP = 1
};

// Floors are numbered 1..4. A value of 0 means position is not yet known.
uint8_t currentFloor = 0;
uint8_t targetFloor = 0;
Direction currentDirection = Direction::STOPPED;
ElevatorState elevatorState = ElevatorState::POSITION_UNKNOWN;
bool positionKnown = false;
bool emergencyLatched = false;

// Keep cabin requests and hall-call requests separate.
bool internalRequests[FLOOR_COUNT] = {false};
bool externalRequests[FLOOR_COUNT] = {false};

// Input snapshots / timing state for later non-blocking modules.
uint8_t pcf8574RawState = 0xFF;       // Latest electrical state from the expander
uint8_t candidatePcf8574State = 0xFF; // State currently being checked for stability
uint8_t previousPcf8574RawState = 0xFF; // Debounced/stable state
unsigned long buttonStateChangeMs[8] = {0};
bool floorSensorStates[FLOOR_COUNT] = {false};

unsigned long lastButtonScanMs = 0;
unsigned long lastPositionScanMs = 0;
unsigned long lastDisplayUpdateMs = 0;
unsigned long lastAudioUpdateMs = 0;

// -----------------------------------------------------------------------------
// Input Manager — PCF8574 button scanning and debouncing
// -----------------------------------------------------------------------------

void registerButtonPress(uint8_t buttonBit) {
  if (buttonBit < 4) {
    externalRequests[buttonBit] = true;
    Serial.print(F("External request: Floor "));
    Serial.println(buttonBit + 1);
  } else {
    const uint8_t floorIndex = buttonBit - 4;
    internalRequests[floorIndex] = true;
    Serial.print(F("Internal request: Floor "));
    Serial.println(floorIndex + 1);
  }
}

void readButtons(unsigned long now) {
  // Ask the PCF8574 for the current state of all eight button inputs.
  if (Wire.requestFrom(PCF8574_ADDRESS, (uint8_t)1) != 1) {
    return;  // Keep the last valid snapshots if the I2C read fails.
  }

  pcf8574RawState = Wire.read();

  // Debounce each button independently. A candidate state must remain unchanged
  // for BUTTON_DEBOUNCE_MS before it becomes the new stable state.
  for (uint8_t buttonBit = 0; buttonBit < 8; buttonBit++) {
    const uint8_t mask = (uint8_t)(1U << buttonBit);
    const bool rawIsHigh = (pcf8574RawState & mask) != 0;
    const bool candidateIsHigh = (candidatePcf8574State & mask) != 0;
    const bool stableIsHigh = (previousPcf8574RawState & mask) != 0;

    if (rawIsHigh != candidateIsHigh) {
      if (rawIsHigh) {
        candidatePcf8574State |= mask;
      } else {
        candidatePcf8574State &= (uint8_t)~mask;
      }
      buttonStateChangeMs[buttonBit] = now;
    }

    const bool updatedCandidateIsHigh = (candidatePcf8574State & mask) != 0;
    if (updatedCandidateIsHigh != stableIsHigh &&
        now - buttonStateChangeMs[buttonBit] >= BUTTON_DEBOUNCE_MS) {
      if (updatedCandidateIsHigh) {
        previousPcf8574RawState |= mask;
      } else {
        previousPcf8574RawState &= (uint8_t)~mask;
        // Only a debounced HIGH -> LOW transition is a new button press.
        registerButtonPress(buttonBit);
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Arduino entry points
// -----------------------------------------------------------------------------

void setup() {
  // Keep the motor disabled before initializing the rest of the system.
  pinMode(MOTOR_ENA_PIN, OUTPUT);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  analogWrite(MOTOR_ENA_PIN, 0);
  digitalWrite(MOTOR_IN1_PIN, LOW);
  digitalWrite(MOTOR_IN2_PIN, LOW);

  // Read the normally-closed emergency circuit.
  pinMode(EMERGENCY_PIN, INPUT);
  emergencyLatched = (digitalRead(EMERGENCY_PIN) == LOW);
  if (emergencyLatched) {
    elevatorState = ElevatorState::EMERGENCY;
  }

  // Speaker output.
  pinMode(SPEAKER_PIN, OUTPUT);
  noTone(SPEAKER_PIN);

  // 74HC595 control pins and a blank common-anode display.
  pinMode(DISPLAY_DATA_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);
  pinMode(DISPLAY_LATCH_PIN, OUTPUT);

  digitalWrite(DISPLAY_LATCH_PIN, LOW);
  shiftOut(DISPLAY_DATA_PIN, DISPLAY_CLOCK_PIN, MSBFIRST, 0xFF);
  digitalWrite(DISPLAY_LATCH_PIN, HIGH);

  // IR sensor outputs are digital inputs (HIGH means detection).
  for (uint8_t i = 0; i < FLOOR_COUNT; i++) {
    pinMode(FLOOR_SENSOR_PINS[i], INPUT);
  }

  // Release all PCF8574 quasi-bidirectional pins before reading the buttons.
  Wire.begin();
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(0xFF);
  const uint8_t pcf8574Status = Wire.endTransmission();

  pcf8574RawState = 0xFF;
  candidatePcf8574State = 0xFF;
  previousPcf8574RawState = 0xFF;

  // Serial is reserved for diagnostics during Proteus testing.
  Serial.begin(9600);
  Serial.println(F("Smart Elevator: startup complete"));
  Serial.println(emergencyLatched ? F("Emergency: ACTIVE") : F("Emergency: NORMAL"));
  Serial.println(pcf8574Status == 0 ? F("PCF8574: FOUND") : F("PCF8574: NOT FOUND"));
  Serial.println(F("Motor output: DISABLED"));

  const unsigned long now = millis();
  for (uint8_t i = 0; i < 8; i++) {
    buttonStateChangeMs[i] = now;
  }
  lastButtonScanMs = now;
  lastPositionScanMs = now;
  lastDisplayUpdateMs = now;
  lastAudioUpdateMs = now;
}

void loop() {
  const unsigned long now = millis();

  // Scan buttons periodically without blocking the rest of the controller.
  if (now - lastButtonScanMs >= BUTTON_SCAN_INTERVAL_MS) {
    lastButtonScanMs = now;
    readButtons(now);
  }
}
