/*
  Smart Elevator Control System
  Arduino Uno R3 / Proteus simulation.
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
constexpr unsigned long POSITION_SCAN_INTERVAL_MS = 20;
constexpr unsigned long SENSOR_DEBOUNCE_MS = 30;
constexpr unsigned long POSITION_STARTUP_CHECK_MS = 50;

// -----------------------------------------------------------------------------
// Arduino Uno pin mapping
// -----------------------------------------------------------------------------

constexpr uint8_t EMERGENCY_PIN = A0;  // NC switch: HIGH = normal, LOW = pressed
constexpr uint8_t SPEAKER_PIN   = A1;

constexpr uint8_t I2C_SDA_PIN = A4;
constexpr uint8_t I2C_SCL_PIN = A5;

constexpr uint8_t DISPLAY_DATA_PIN  = 2;
constexpr uint8_t DISPLAY_CLOCK_PIN = 3;
constexpr uint8_t DISPLAY_LATCH_PIN = 4;

constexpr uint8_t MOTOR_IN1_PIN = 5;
constexpr uint8_t MOTOR_IN2_PIN = 6;
constexpr uint8_t MOTOR_ENA_PIN = 9;
constexpr uint8_t DEFAULT_MOTOR_PWM = 150;

const uint8_t FLOOR_SENSOR_PINS[FLOOR_COUNT] = {10, 11, 12, 13};
constexpr uint8_t FLOOR_SENSOR_ACTIVE_LEVEL = HIGH;

// -----------------------------------------------------------------------------
// PCF8574 button mapping
// -----------------------------------------------------------------------------

enum ButtonBit : uint8_t {
  EXTERNAL_FLOOR_1 = 0,
  EXTERNAL_FLOOR_2 = 1,
  EXTERNAL_FLOOR_3 = 2,
  EXTERNAL_FLOOR_4 = 3,
  INTERNAL_FLOOR_1 = 4,
  INTERNAL_FLOOR_2 = 5,
  INTERNAL_FLOOR_3 = 6,
  INTERNAL_FLOOR_4 = 7
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

// Floors are numbered 1..4. Zero means that the position is unknown.
uint8_t currentFloor = 0;
uint8_t targetFloor = 0;
Direction currentDirection = Direction::STOPPED;
ElevatorState elevatorState = ElevatorState::POSITION_UNKNOWN;
bool positionKnown = false;
bool emergencyLatched = false;

bool internalRequests[FLOOR_COUNT] = {false};
bool externalRequests[FLOOR_COUNT] = {false};

uint8_t pcf8574RawState = 0xFF;
uint8_t candidatePcf8574State = 0xFF;
uint8_t previousPcf8574RawState = 0xFF;
unsigned long buttonStateChangeMs[8] = {0};

bool floorSensorStates[FLOOR_COUNT] = {false};
bool candidateFloorSensorStates[FLOOR_COUNT] = {false};
unsigned long floorSensorChangeMs[FLOOR_COUNT] = {0};
bool positionSensorsInitialized = false;
unsigned long positionStartupCheckMs = 0;

unsigned long lastButtonScanMs = 0;
unsigned long lastPositionScanMs = 0;
unsigned long lastSchedulingUpdateMs = 0;
unsigned long lastDisplayUpdateMs = 0;
unsigned long lastAudioUpdateMs = 0;

// -----------------------------------------------------------------------------
// Request Manager — stores and clears floor requests
// -----------------------------------------------------------------------------

void addInternalRequest(uint8_t floorIndex) {
  if (floorIndex < FLOOR_COUNT) {
    internalRequests[floorIndex] = true;
  }
}

void addExternalRequest(uint8_t floorIndex) {
  if (floorIndex < FLOOR_COUNT) {
    externalRequests[floorIndex] = true;
  }
}

bool hasRequests() {
  for (uint8_t floorIndex = 0; floorIndex < FLOOR_COUNT; floorIndex++) {
    if (internalRequests[floorIndex] || externalRequests[floorIndex]) {
      return true;
    }
  }
  return false;
}

void clearFloorRequests(uint8_t floorIndex) {
  if (floorIndex >= FLOOR_COUNT) {
    return;
  }

  const bool hadRequest =
      internalRequests[floorIndex] || externalRequests[floorIndex];

  internalRequests[floorIndex] = false;
  externalRequests[floorIndex] = false;

  if (hadRequest) {
    Serial.print(F("Requests cleared at Floor "));
    Serial.println(floorIndex + 1);
  }
}

// -----------------------------------------------------------------------------
// Input Manager — PCF8574 buttons with per-button debouncing
// -----------------------------------------------------------------------------

void registerButtonPress(uint8_t buttonBit) {
  if (buttonBit < 4) {
    addExternalRequest(buttonBit);
    Serial.print(F("External request: Floor "));
    Serial.println(buttonBit + 1);
  } else {
    const uint8_t floorIndex = buttonBit - 4;
    addInternalRequest(floorIndex);
    Serial.print(F("Internal request: Floor "));
    Serial.println(floorIndex + 1);
  }
}

void readButtons(unsigned long now) {
  if (Wire.requestFrom(PCF8574_ADDRESS, (uint8_t)1) != 1) {
    return;
  }

  pcf8574RawState = Wire.read();

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
        registerButtonPress(buttonBit);
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Position Manager — four IR floor sensors
// -----------------------------------------------------------------------------

void readFloorSensors(unsigned long now) {
  bool allSensorsStable = true;

  for (uint8_t floorIndex = 0; floorIndex < FLOOR_COUNT; floorIndex++) {
    const bool rawDetected =
        (digitalRead(FLOOR_SENSOR_PINS[floorIndex]) == FLOOR_SENSOR_ACTIVE_LEVEL);

    if (rawDetected != candidateFloorSensorStates[floorIndex]) {
      candidateFloorSensorStates[floorIndex] = rawDetected;
      floorSensorChangeMs[floorIndex] = now;
    }

    if (candidateFloorSensorStates[floorIndex] != floorSensorStates[floorIndex]) {
      if (now - floorSensorChangeMs[floorIndex] >= SENSOR_DEBOUNCE_MS) {
        const bool wasDetected = floorSensorStates[floorIndex];
        floorSensorStates[floorIndex] = candidateFloorSensorStates[floorIndex];

        // After startup calibration, only a new inactive -> active transition
        // represents a newly reached floor.
        if (positionSensorsInitialized &&
            !wasDetected && floorSensorStates[floorIndex]) {
          currentFloor = floorIndex + 1;
          positionKnown = true;

          Serial.print(F("Position detected: Floor "));
          Serial.println(currentFloor);
        }
      } else {
        allSensorsStable = false;
      }
    }
  }

  // At startup, wait until every sensor has settled before determining whether
  // the cabin is already aligned with exactly one floor sensor.
  if (!positionSensorsInitialized &&
      allSensorsStable &&
      now - positionStartupCheckMs >= POSITION_STARTUP_CHECK_MS) {
    uint8_t detectedCount = 0;
    uint8_t detectedFloor = 0;

    for (uint8_t floorIndex = 0; floorIndex < FLOOR_COUNT; floorIndex++) {
      if (floorSensorStates[floorIndex]) {
        detectedCount++;
        detectedFloor = floorIndex + 1;
      }
    }

    if (detectedCount == 1) {
      currentFloor = detectedFloor;
      positionKnown = true;
      Serial.print(F("Initial position: Floor "));
      Serial.println(currentFloor);
    } else {
      currentFloor = 0;
      positionKnown = false;

      if (detectedCount == 0) {
        Serial.println(F("Initial position: UNKNOWN (no floor sensor active)"));
      } else {
        Serial.println(F("Initial position: UNKNOWN (multiple floor sensors active)"));
      }
    }

    positionSensorsInitialized = true;
  }
}

// -----------------------------------------------------------------------------
// Scheduling Manager — LOOK-style directional request selection
// -----------------------------------------------------------------------------

constexpr unsigned long SCHEDULING_INTERVAL_MS = 100;

bool isFloorRequested(uint8_t floorIndex) {
  if (floorIndex >= FLOOR_COUNT) {
    return false;
  }

  return internalRequests[floorIndex] || externalRequests[floorIndex];
}

uint8_t findNextRequestedFloor() {
  if (!positionKnown || currentFloor < 1 || currentFloor > FLOOR_COUNT) {
    return 0;
  }

  // Continue in the current direction first.
  if (currentDirection == Direction::UP) {
    for (uint8_t floor = currentFloor + 1; floor <= FLOOR_COUNT; floor++) {
      if (isFloorRequested(floor - 1)) {
        return floor;
      }
    }
  } else if (currentDirection == Direction::DOWN) {
    for (int8_t floor = currentFloor - 1; floor >= 1; floor--) {
      if (isFloorRequested((uint8_t)(floor - 1))) {
        return (uint8_t)floor;
      }
    }
  }

  // No request remains ahead: look in the opposite direction.
  if (currentDirection == Direction::UP) {
    for (int8_t floor = currentFloor - 1; floor >= 1; floor--) {
      if (isFloorRequested((uint8_t)(floor - 1))) {
        return (uint8_t)floor;
      }
    }
  } else if (currentDirection == Direction::DOWN) {
    for (uint8_t floor = currentFloor + 1; floor <= FLOOR_COUNT; floor++) {
      if (isFloorRequested(floor - 1)) {
        return floor;
      }
    }
  } else {
    // When idle, choose the closest pending floor. Ties go to the lower floor.
    uint8_t closestFloor = 0;
    uint8_t closestDistance = FLOOR_COUNT + 1;

    for (uint8_t floor = 1; floor <= FLOOR_COUNT; floor++) {
      if (!isFloorRequested(floor - 1)) {
        continue;
      }

      const uint8_t distance = (floor > currentFloor)
          ? floor - currentFloor
          : currentFloor - floor;

      if (distance < closestDistance) {
        closestDistance = distance;
        closestFloor = floor;
      }
    }

    return closestFloor;
  }

  return 0;
}

void updateScheduling() {
  if (emergencyLatched || elevatorState == ElevatorState::EMERGENCY) {
    return;
  }

  if (!positionKnown || currentFloor < 1 || currentFloor > FLOOR_COUNT) {
    targetFloor = 0;
    currentDirection = Direction::STOPPED;
    elevatorState = ElevatorState::POSITION_UNKNOWN;
    return;
  }

  const uint8_t nextFloor = findNextRequestedFloor();

  if (nextFloor == 0) {
    targetFloor = 0;
    currentDirection = Direction::STOPPED;
    elevatorState = ElevatorState::IDLE;
    return;
  }

  const uint8_t previousTarget = targetFloor;
  const Direction previousDirection = currentDirection;

  targetFloor = nextFloor;
  currentDirection = (targetFloor > currentFloor)
      ? Direction::UP
      : Direction::DOWN;
  elevatorState = (currentDirection == Direction::UP)
      ? ElevatorState::MOVING_UP
      : ElevatorState::MOVING_DOWN;

  if (targetFloor != previousTarget || currentDirection != previousDirection) {
    Serial.print(F("Next target: Floor "));
    Serial.print(targetFloor);
    Serial.print(F(" | Direction: "));
    Serial.println(currentDirection == Direction::UP ? F("UP") : F("DOWN"));
  }
}

// -----------------------------------------------------------------------------
// Arduino entry points
// -----------------------------------------------------------------------------

void setup() {
  // Disable the motor before initializing the rest of the system.
  pinMode(MOTOR_ENA_PIN, OUTPUT);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  analogWrite(MOTOR_ENA_PIN, 0);
  digitalWrite(MOTOR_IN1_PIN, LOW);
  digitalWrite(MOTOR_IN2_PIN, LOW);

  pinMode(EMERGENCY_PIN, INPUT);
  emergencyLatched = (digitalRead(EMERGENCY_PIN) == LOW);
  if (emergencyLatched) {
    elevatorState = ElevatorState::EMERGENCY;
  }

  pinMode(SPEAKER_PIN, OUTPUT);
  noTone(SPEAKER_PIN);

  pinMode(DISPLAY_DATA_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);
  pinMode(DISPLAY_LATCH_PIN, OUTPUT);
  digitalWrite(DISPLAY_LATCH_PIN, LOW);
  shiftOut(DISPLAY_DATA_PIN, DISPLAY_CLOCK_PIN, MSBFIRST, 0xFF);
  digitalWrite(DISPLAY_LATCH_PIN, HIGH);

  for (uint8_t i = 0; i < FLOOR_COUNT; i++) {
    pinMode(FLOOR_SENSOR_PINS[i], INPUT);
  }

  Wire.begin();
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(0xFF);
  const uint8_t pcf8574Status = Wire.endTransmission();

  pcf8574RawState = 0xFF;
  candidatePcf8574State = 0xFF;
  previousPcf8574RawState = 0xFF;

  Serial.begin(9600);
  Serial.println(F("Smart Elevator: startup complete"));
  Serial.println(emergencyLatched ? F("Emergency: ACTIVE") : F("Emergency: NORMAL"));
  Serial.println(pcf8574Status == 0 ? F("PCF8574: FOUND") : F("PCF8574: NOT FOUND"));
  Serial.println(F("Motor output: DISABLED"));

  const unsigned long now = millis();
  for (uint8_t i = 0; i < 8; i++) {
    buttonStateChangeMs[i] = now;
  }
  for (uint8_t i = 0; i < FLOOR_COUNT; i++) {
    floorSensorChangeMs[i] = now;
  }

  positionStartupCheckMs = now;
  lastButtonScanMs = now;
  lastPositionScanMs = now;
  lastSchedulingUpdateMs = now;
  lastDisplayUpdateMs = now;
  lastAudioUpdateMs = now;
}

void loop() {
  const unsigned long now = millis();

  if (now - lastButtonScanMs >= BUTTON_SCAN_INTERVAL_MS) {
    lastButtonScanMs = now;
    readButtons(now);
  }

  if (now - lastPositionScanMs >= POSITION_SCAN_INTERVAL_MS) {
    lastPositionScanMs = now;
    readFloorSensors(now);
  }

  if (now - lastSchedulingUpdateMs >= SCHEDULING_INTERVAL_MS) {
    lastSchedulingUpdateMs = now;
    updateScheduling();
  }
}
