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
uint8_t pcf8574RawState = 0xFF;  // HIGH means released; LOW means pressed
uint8_t previousPcf8574RawState = 0xFF;
bool floorSensorStates[FLOOR_COUNT] = {false};

unsigned long lastButtonScanMs = 0;
unsigned long lastPositionScanMs = 0;
unsigned long lastDisplayUpdateMs = 0;
unsigned long lastAudioUpdateMs = 0;

// -----------------------------------------------------------------------------
// Arduino entry points
// -----------------------------------------------------------------------------

void setup() {
  // Hardware initialization will be added in the next implementation step.
}

void loop() {
  // Non-blocking module updates will be coordinated here.
}
