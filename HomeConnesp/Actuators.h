#pragma once
#include <Arduino.h>

namespace Actuators {
    void init(uint8_t relayPin, uint8_t servoPin);
    void update();

    void setRelay(bool on);
    bool getRelay();

    void setServo(uint8_t angle);  // 0-180
    uint8_t getServo();

    void setAutoMode(bool en);
    bool getAutoMode();
}
