#include "Actuators.h"
#include "Sensors.h"
#include "Logger.h"

// Servo via LEDC PWM (no external library needed)
#define SERVO_CHANNEL  0
#define SERVO_FREQ     50
#define SERVO_RES      16
#define SERVO_MIN_US   500
#define SERVO_MAX_US   2400

static uint8_t  _relayPin;
static uint8_t  _servoPin;
static bool     _relayOn    = false;
static uint8_t  _servoAngle = 90;
static bool     _autoMode   = true;

static uint32_t angleToTicks(uint8_t angle) {
    uint32_t us = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
    return (uint32_t)((us / 20000.0f) * 65535);
}

void Actuators::init(uint8_t relayPin, uint8_t servoPin) {
    _relayPin = relayPin;
    _servoPin = servoPin;

    pinMode(_relayPin, OUTPUT);
    digitalWrite(_relayPin, LOW);

    ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
    ledcAttachPin(_servoPin, SERVO_CHANNEL);
    ledcWrite(SERVO_CHANNEL, angleToTicks(90));
}

void Actuators::update() {
    if (!_autoMode) return;
    // Auto: motion → relay on; no motion for 10s → relay off
    if (Sensors::getMotion()) {
        if (!_relayOn) {
            _relayOn = true;
            digitalWrite(_relayPin, HIGH);
            Logger::log(LOG_INFO, "Auto: relé ligado por movimento");
        }
    } else {
        uint32_t elapsed = millis() - Sensors::getLastMotionTime();
        if (_relayOn && elapsed > 10000) {
            _relayOn = false;
            digitalWrite(_relayPin, LOW);
            Logger::log(LOG_INFO, "Auto: relé desligado (sem movimento)");
        }
    }
    // Auto: door open → servo unlock (0°); door closed → servo lock (90°)
    bool doorOpen = Sensors::getDoor();
    uint8_t target = doorOpen ? 0 : 90;
    if (_servoAngle != target) {
        _servoAngle = target;
        ledcWrite(SERVO_CHANNEL, angleToTicks(_servoAngle));
        Logger::log(LOG_INFO, doorOpen ? "Auto: tranca aberta" : "Auto: tranca fechada");
    }
}

void Actuators::setRelay(bool on) {
    _relayOn = on;
    digitalWrite(_relayPin, on ? HIGH : LOW);
}
bool Actuators::getRelay() { return _relayOn; }

void Actuators::setServo(uint8_t angle) {
    _servoAngle = constrain(angle, 0, 180);
    ledcWrite(SERVO_CHANNEL, angleToTicks(_servoAngle));
}
uint8_t Actuators::getServo() { return _servoAngle; }

void Actuators::setAutoMode(bool en) { _autoMode = en; }
bool Actuators::getAutoMode()        { return _autoMode; }
