#include "Sensors.h"

static uint8_t  _pirPin, _reedPin;
static volatile bool _motion    = false;
static volatile bool _doorOpen  = false;
static volatile uint32_t _lastMotion = 0;
static volatile uint32_t _lastDoor   = 0;

void Sensors::init(uint8_t pirPin, uint8_t reedPin) {
    _pirPin  = pirPin;
    _reedPin = reedPin;
    pinMode(_pirPin,  INPUT);
    pinMode(_reedPin, INPUT_PULLUP);
}

bool Sensors::getMotion()          { return _motion; }
bool Sensors::getDoor()            { return _doorOpen; }
uint32_t Sensors::getLastMotionTime() { return _lastMotion; }
uint32_t Sensors::getLastDoorTime()   { return _lastDoor; }

void Sensors::setMotion(bool v) {
    _motion = v;
    if (v) _lastMotion = millis();
}

void Sensors::setDoor(bool v) {
    _doorOpen = v;
    _lastDoor = millis();
}
