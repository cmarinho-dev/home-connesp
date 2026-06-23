#include "Actuators.h"
#include "Sensors.h"
#include "Logger.h"

#include <ESP32Servo.h>

// Servo parameters (using ESP32Servo library)
#define SERVO_MIN_US      500
#define SERVO_MAX_US      2400
#define SERVO_CLOSED_ANGLE 60
#define SERVO_OPEN_ANGLE   150

static uint8_t  _relayPin;
static uint8_t  _servoPin;
static bool     _relayOn    = false;
static uint8_t  _servoAngle = SERVO_CLOSED_ANGLE;
static bool     _autoMode   = true;
static Servo    _servo;

// angleToTicks removed — using Servo library
// Most relay modules for ESP32 are active-low: LOW energizes the relay.
static constexpr bool RELAY_ACTIVE_LOW = true;

static void writeRelay(bool on) {
    digitalWrite(_relayPin, RELAY_ACTIVE_LOW ? (on ? LOW : HIGH) : (on ? HIGH : LOW));
}

void Actuators::init(uint8_t relayPin, uint8_t servoPin) {
    _relayPin = relayPin;
    _servoPin = servoPin;

    pinMode(_relayPin, OUTPUT);
    _relayOn = false;
    writeRelay(_relayOn);

    // Attach servo using ESP32Servo (with microseconds range)
    _servo.attach(_servoPin, SERVO_MIN_US, SERVO_MAX_US);
    _servo.write(_servoAngle);
}

void Actuators::update() {
    if (!_autoMode) return;
    // Auto: motion → relay on; no motion for 10s → relay off
    if (Sensors::getMotion()) {
        if (!_relayOn) {
            _relayOn = true;
            writeRelay(_relayOn);
            Logger::log(LOG_INFO, "Auto: relé ligado por movimento");
        }
    } else {
        uint32_t elapsed = millis() - Sensors::getLastMotionTime();
        if (_relayOn && elapsed > 10000) {
            _relayOn = false;
            writeRelay(_relayOn);
            Logger::log(LOG_INFO, "Auto: relé desligado (sem movimento)");
        }
    }
    // Auto: door open -> servo unlock (150 deg); door closed -> servo lock (60 deg)
    bool doorOpen = Sensors::getDoor();
    uint8_t target = doorOpen ? SERVO_OPEN_ANGLE : SERVO_CLOSED_ANGLE;
    if (_servoAngle != target) {
        _servoAngle = target;
        _servo.write(_servoAngle);
        Logger::log(LOG_INFO, doorOpen ? "Auto: tranca aberta" : "Auto: tranca fechada");
    }
}

void Actuators::setRelay(bool on) {
    _relayOn = on;
    writeRelay(_relayOn);
}
bool Actuators::getRelay() { return _relayOn; }

void Actuators::setServo(uint8_t angle) {
    _servoAngle = constrain(angle, SERVO_CLOSED_ANGLE, SERVO_OPEN_ANGLE);
    _servo.write(_servoAngle);
}
uint8_t Actuators::getServo() { return _servoAngle; }

void Actuators::setAutoMode(bool en) { _autoMode = en; }
bool Actuators::getAutoMode()        { return _autoMode; }
